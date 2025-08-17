import os
import sys
import utils
import logging
import shutil
from third_party import platforms
from third_party import build_unittests_foldername
from itertools import product
from third_party import prefered


def prepare(node, parameters, compiler_replace_maps):

    package = node.get_package_name()

    # source folder
    source_dir = os.path.join(os.getcwd(), package)
    utils.trymkdir(source_dir)

    # generate .build.sh / .build.cmd if is defined in yaml
    node.get_generate_custom_script(source_dir)

    # generate find.script / find.cmd
    node.generate_scripts_headers(compiler_replace_maps)

    # read root CMakeLists.txt
    with open('CMakeLists.txt', 'rt') as f:
        content_cmakelists = f.read()

    # OJO: dejar de borrar cuando reciclemos binarios
    node.remove_packages()

    # run_tests or packing
    build_modes = node.get_build_modes()
    for plat, build_mode in product(platforms, build_modes):
        logging.info('Preparing mode %s - %s' % (plat, build_mode))
        build_directory = os.path.join(os.getcwd(), node.get_build_directory(plat, build_mode))
        utils.trymkdir(build_directory)

        # download source and prepare in build_directory
        node.prepare_third_party(build_directory, compiler_replace_maps)

        # copy source files to build
        logging.debug('Copy sources to build: %s -> %s' % (source_dir, build_directory))
        utils.copy_folder_recursive(source_dir, build_directory)

        # before copy files
        with utils.working_directory(build_directory):
            for bc in node.get_before_copy():
                chunks = [x.strip() for x in bc.split(' ') if x]
                if len(chunks) != 2:
                    raise Exception('Invalid value in before_copy: %s' % bc)
                logging.debug('Copy "%s" to "%s"' % (chunks[0], chunks[1]))
                shutil.copy2(chunks[0], chunks[1])

        # if have cmakelists, insert root cmakelists header
        cmake_prefix = node.get_cmake_prefix()
        build_cmakelist = os.path.join(build_directory, cmake_prefix, 'CMakeLists.txt')
        if os.path.exists(build_cmakelist) and (not node.has_custom_script(source_dir)):
            with open(build_cmakelist, 'rt') as f:
                content_cmakelists_package = f.read()
            with open(build_cmakelist, 'wt') as f:
                f.write('%s\n' % content_cmakelists)
                f.write('%s\n' % content_cmakelists_package)

        if parameters.fast:
            logging.debug('skipping for because is in fast mode: "prepare"')
            break

    # finish well
    return True

