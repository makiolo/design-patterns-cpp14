import os
import sys
import utils
import logging
import hash_version
from itertools import product
from third_party import platforms
from third_party import get_identifier


def print_folder(folder):
    for root, dirs, files in os.walk(folder):
        path = root.split(os.sep)
        logging.info((len(path) - 1) * '... ' + '%s/' % os.path.basename(root))
        for file in files:
            logging.info(len(path) * '... ' + '%s' % file)


def packing(node, parameters, compiler_replace_maps):

    package = node.get_package_name()
    version_git = node.get_version()
    packing = node.is_packing()
    if not packing:
        logging.warning('Skiping package: %s' % package)
        return 0

    manager = node.get_version_manager()
    if manager == "git":
        build_modes = node.get_build_modes()
        for plat, build_mode in product(platforms, build_modes):
            build_directory = os.path.join(os.getcwd(), node.get_build_directory(plat, build_mode))
            revision_git = hash_version.get_last_changeset(build_directory, short=False)
            version_old = node.get_version()
            version_git = hash_version.to_cmaki_version(build_directory, revision_git)
            logging.info('[git] Renamed version from %s to %s' % (version_old, version_git))

            current_workspace = node.get_binary_workspace(plat)
            current_base = node.get_base_folder()
            oldversion = node.get_version()
            try:
                node.set_version(version_git)
                updated_workspace = node.get_binary_workspace(plat)
                updated_base = node.get_base_folder()

                current_base2 = os.path.join(current_workspace, current_base)
                updated_base2 = os.path.join(current_workspace, updated_base)
                logging.debug("from: %s" % current_base2)
                logging.debug("to: %s" % updated_base2)
                if current_base != updated_base:
                    utils.move_folder_recursive(current_base2, updated_base2)
                    logging.debug('-- copy from: {}, {}'.format(current_workspace, os.path.exists(current_workspace)))
                    logging.debug('-- copy to: {}, {}'.format(updated_workspace, os.path.exists(updated_workspace)))
                    utils.move_folder_recursive(current_workspace, updated_workspace)
            finally:
                node.set_version(oldversion)

    node.set_version(version_git)
    version = node.get_version()

    # regenerate autoscripts with new version
    node.generate_scripts_headers(compiler_replace_maps)

    # # generate versions.cmake
    node.generate_3rdpartyversion(parameters.prefix)

    precmd = ''
    if utils.is_windows():
        precmd = 'cmake -E '

    folder_3rdparty = parameters.third_party_dir
    output_3rdparty = os.path.join(folder_3rdparty, node.get_base_folder())
    utils.trymkdir(output_3rdparty)

    folder_mark = os.path.join(parameters.prefix, node.get_base_folder())
    utils.trymkdir(folder_mark)

    utils.superverbose(parameters, '*** [%s] Generation cmakefiles *** %s' % (package, output_3rdparty))
    errors = node.generate_cmakefiles(platforms, output_3rdparty, compiler_replace_maps)
    logging.debug('errors generating cmakefiles: %d' % errors)
    node.ret += abs(errors)

    for plat in platforms:
        utils.superverbose(parameters, '*** [%s (%s)] Generating package .tar.gz (%s) ***' % (package, version, plat))
        workspace = node.get_workspace(plat)
        current_workspace = node.get_binary_workspace(plat)
        utils.trymkdir(current_workspace)
        with utils.working_directory(current_workspace):

            logging.info('working directory: {}'.format(current_workspace))

            if utils.is_windows():
                utils.safe_system('del /s *.ilk')
                utils.safe_system('del /s *.exp')

            current_base = node.get_base_folder()
            prefix_package = os.path.join(parameters.prefix, '%s.tar.gz' % workspace)
            prefix_package_md5 = os.path.join(output_3rdparty, '%s.md5' % workspace)

            logging.info('generating package %s from source %s' % (prefix_package, os.path.join(os.getcwd(), current_base)))
            logging.info('generating md5file %s' % prefix_package_md5)
            print_folder(current_base)

            # packing install
            gen_targz = "%star zcvf %s %s" % (precmd, prefix_package, current_base)

            node.ret += abs( node.safe_system(gen_targz, compiler_replace_maps) )
            if not os.path.exists(prefix_package):
                logging.error('No such file: {}'.format(prefix_package))
                return False

            # calculate md5 file
            package_md5 = utils.md5sum(prefix_package)
            logging.debug("new package {}, with md5sum {}".format(prefix_package, package_md5))
            with open(prefix_package_md5, 'wt') as f:
                f.write('%s\n' % package_md5)

    # packing cmakefiles (more easy distribution)
    if not parameters.no_packing_cmakefiles:
        for plat in platforms:
            current_base = node.get_base_folder()
            prefix_package_cmake = os.path.join(parameters.prefix, '%s-%s-cmake.tar.gz' % (current_base, plat))
            with utils.working_directory(folder_3rdparty):

                logging.info('working directory: {}'.format(folder_3rdparty))

                logging.debug('working dir: %s' % folder_3rdparty)
                logging.info('generating package cmake %s' % prefix_package_cmake)
                print_folder(current_base)

                gen_targz_cmake = '{}tar zcvf {} {}'.format(precmd, prefix_package_cmake, current_base)
                node.ret += abs( node.safe_system(gen_targz_cmake, compiler_replace_maps) )
                if not os.path.exists(prefix_package_cmake):
                    logging.error('No such file: {}'.format(prefix_package_cmake))
                    return False

    # finish well
    return True

