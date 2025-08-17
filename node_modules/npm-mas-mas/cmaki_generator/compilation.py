import os
import utils
import logging
import pipeline
from itertools import product
from third_party import platforms
from third_party import CMAKELIB_URL
from third_party import get_identifier


def search_cmakelib():
    # compilando desde cmaki_generator
    cmakelib_dir = os.path.join('output', '3rdparties', 'cmaki')
    if not os.path.isdir(cmakelib_dir):
        # compilando una dependencia
        cmakelib_dir = os.path.join('..', 'cmaki')
        if not os.path.isdir(cmakelib_dir):
            # compilando proeycto raiz
            cmakelib_dir = os.path.join('node_modules', 'cmaki')
            if not os.path.isdir(cmakelib_dir):
                raise Exception("not found cmaki: {}".format(os.path.abspath(cmakelib_dir)))
    return os.path.abspath(cmakelib_dir)


def compilation(node, parameters, compiler_replace_maps):

    package = node.get_package_name()
    package_norm = node.get_package_name_norm()
    version = node.get_version()

    cmake3p_dir = parameters.prefix
    cmake3p_dir = utils.get_norm_path(cmake3p_dir)
    cmake3p_dir = cmake3p_dir.replace('\\', '/')

    cmakefiles_dir = search_cmakelib()

    package_upper = node.get_package_name_norm_upper()
    parms = node.parameters
    build_modes = node.get_build_modes()
    for plat, build_mode in product(platforms, build_modes):
        install_directory = node.get_install_directory(plat)
        utils.trymkdir(install_directory)
        build_directory = os.path.join(os.getcwd(), node.get_build_directory(plat, build_mode))
        utils.trymkdir(build_directory)
        with utils.working_directory(build_directory):
            # get generator and platform info
            for compiler_c, compiler_cpp, generator, _, _, env_modified, _ in node.compiler_iterator(plat, compiler_replace_maps):

                logging.info('-- compilation mode: %s plat: %s' % (build_mode, plat))

                ############# 1. prepare vars

                if build_mode.lower() == 'debug':
                    try:
                        env_modified['CFLAGS'] = '%s -g -O0 -D_DEBUG -DDEBUG' % env_modified['CFLAGS']
                    except KeyError:
                        env_modified['CFLAGS'] = '-g -O0 -D_DEBUG -DDEBUG'
                    try:
                        env_modified['CPPFLAGS'] = '%s -g -O0 -D_DEBUG -DDEBUG' % env_modified['CPPFLAGS']
                    except KeyError:
                        env_modified['CPPFLAGS'] = '-g -O0 -D_DEBUG -DDEBUG'
                elif build_mode.lower() == 'relwithdebinfo':
                    try:
                        env_modified['CFLAGS'] = '%s -g -O2 -DNDEBUG' % env_modified['CFLAGS']
                    except KeyError:
                        env_modified['CFLAGS'] = '-g -O2 -DNDEBUG'
                    try:
                        env_modified['CPPFLAGS'] = '%s -g -O2 -DNDEBUG' % env_modified['CPPFLAGS']
                    except KeyError:
                        env_modified['CPPFLAGS'] = '-g -O2 -DNDEBUG'
                elif build_mode.lower() == 'release':
                    # default packages assume came in release
                    try:
                        env_modified['CFLAGS'] = '%s -O3 -DNDEBUG' % env_modified['CFLAGS']
                    except KeyError:
                        env_modified['CFLAGS'] = '-O3 -DNDEBUG'
                    try:
                        env_modified['CPPFLAGS'] = '%s -O3 -DNDEBUG' % env_modified['CPPFLAGS']
                    except KeyError:
                        env_modified['CPPFLAGS'] = '-O3 -DNDEBUG'

                cores = utils.detect_ncpus()
                half_cores = cores / 2
                env_modified['CORES'] = str(cores)
                env_modified['HALF_CORES'] = str(half_cores)
                env_modified['GTC_PREFIX'] = parameters.prefix
                env_modified['CMAKELIB_URL'] = CMAKELIB_URL
                env_modified['BUILD_MODE'] = str(build_mode)
                # env_modified['NPP_SERVER'] = ...
                env_modified['SOURCES'] = os.path.abspath(os.path.join('..', node.get_download_directory()))
                env_modified['CMAKI_DIR'] = cmakefiles_dir
                env_modified['SELFHOME'] = install_directory
                env_modified['CMAKI_PWD'] = build_directory
                env_modified['CMAKI_INSTALL'] = install_directory

                #################
                # remove cmake3p of node
                node.remove_cmake3p(cmake3p_dir)

                # show env vars
                node.show_environment_vars(env_modified)

                # remove CMakeCache.txt for avoid problems when
                # change of generator
                utils.tryremove('CMakeCache.txt')
                utils.tryremove('cmake_install.cmake')
                utils.tryremove('install_manifest.txt')
                utils.tryremove_dir('CMakeFiles')
                #################

                generator_extra = ''
                if generator is not None:
                    generator_extra = '-G"%s"' % generator

                cmakefiles_dir = parameters.cmakefiles
                cmakefiles_dir = cmakefiles_dir.replace('\\', '/')

                cmake_prefix_path = parameters.third_party_dir
                cmake_prefix_path = cmake_prefix_path.replace('\\', '/')

                build_directory = build_directory.replace('\\', '/')

                # resolve replace maps
                compiler_replace_resolved = {}
                for var, value in compiler_replace_maps.items():
                    newvalue = value
                    newvalue = newvalue.replace('$PLATFORM', plat)
                    compiler_replace_resolved[var] = newvalue

                # begin definitions cmake
                try:
                    cmake_definitions_list_original = parms['cmake_definitions']
                    cmake_definitions_list = []
                    for define in cmake_definitions_list_original:
                        # TODO: resolver tus variables directas e indirectas (de dependencias)
                        define = define.replace('$%s_HOME' % package_norm, install_directory)
                        # apply replaces
                        cmake_definitions_list.append( utils.apply_replaces(define, compiler_replace_resolved) )
                except KeyError:
                    cmake_definitions_list = []

                # add cflags and cppflags to cmake_definitions
                try:
                    cmake_definitions_list.append( 'CMAKE_C_FLAGS="%s"' % env_modified['CFLAGS'] )
                except KeyError:
                    pass
                try:
                    cmake_definitions_list.append( 'CMAKE_CXX_FLAGS="%s"' % env_modified['CPPFLAGS'] )
                except KeyError:
                    pass

                definitions_extra = ''
                for definition in cmake_definitions_list:
                    definitions_extra += ' -D%s' % definition
                # end definitions cmake

                if (not 'CMAKE_TOOLCHAIN_FILE' in env_modified) or (not env_modified['CMAKE_TOOLCHAIN_FILE']) or (env_modified['CMAKE_TOOLCHAIN_FILE'] == "no cross compile"):
                    cmake_toolchain_file_filepath=''
                else:
                    cmake_toolchain_file_filepath=' -DCMAKE_TOOLCHAIN_FILE="{}"'.format(env_modified['CMAKE_TOOLCHAIN_FILE'])

                cmake_prefix = node.get_cmake_prefix()
                cmake_configure = 'cmake %s %s -DNPP_ARTIFACTS_PATH="%s" -DCMAKE_MODULE_PATH=%s -DCMAKI_PATH=%s -DCMAKE_BUILD_TYPE=%s -DCMAKE_PREFIX_PATH=%s -DPACKAGE=%s -DPACKAGE_UPPER=%s -DPACKAGE_VERSION=%s -DPACKAGE_BUILD_DIRECTORY=%s -DCMAKI_COMPILER=%s -DCMAKI_IDENTIFIER=%s -DCMAKI_PLATFORM=%s %s %s' % (generator_extra, cmake_prefix, cmake3p_dir, cmakefiles_dir, cmakefiles_dir, build_mode, cmake_prefix_path, package, package_upper, version, build_directory, get_identifier('COMPILER'), get_identifier('ALL'), get_identifier('ALL'), definitions_extra, cmake_toolchain_file_filepath)

                target = node.get_cmake_target()
                if target is not None:
                    cmake_build = 'cmake --build . --target %s --config %s' % (target, build_mode)
                else:
                    cmake_build = 'cmake --build . --config %s' % build_mode

                env_modified['CMAKE_CONFIGURE'] = cmake_configure.replace(r'"', r"'")
                env_modified['CMAKE_BUILD'] = cmake_build.replace(r'"', r"'")

                ########## 2. execute

                executed_build_script = False
                if utils.is_windows():
                    for build_script in ['.build.cmd', 'build.cmd']:
                        if os.path.exists(build_script):
                            # execute manual build script
                            node.ret += abs(utils.safe_system('%s %s %s %s %s %s' % (build_script, install_directory, package, version, plat, build_mode), env=env_modified))
                            executed_build_script = True
                else:
                    for build_script in ['.build.sh', 'build.sh']:
                        if os.path.exists(build_script):
                            # show vars
                            node.show_environment_vars(env_modified)

                            node.ret += abs(utils.safe_system('chmod +x %s && ./%s %s %s %s %s %s' % (build_script, build_script, install_directory, package, version, plat, build_mode), env=env_modified))
                            executed_build_script = True

                if not executed_build_script:
                    logging.debug('configure command: %s' % cmake_configure)

                    ret = utils.safe_system(cmake_configure, env=env_modified)
                    if ret == 0:
                        logging.debug('build command: %s' % cmake_configure)
                        node.ret += abs(utils.safe_system(cmake_build, env=env_modified))
                    else:
                        logging.warning('Configuration failed. See log: %s' % parameters.log)
                        node.ret += abs(ret)

                ######## 3. manual install

                # post-install
                logging.debug('begin post-install')
                for bc in node.get_post_install():
                    chunks = [x.strip() for x in bc.split(' ') if x]
                    if(len(chunks) != 2) and (len(chunks) != 3):
                        raise Exception('Invalid value in post_install: %s. Expected [source pattern destiny]' % bc)

                    source_folder = os.path.join(build_directory, os.path.dirname(chunks[0]))
                    install_directory_chunk = os.path.join(install_directory, chunks[1])
                    pattern = os.path.basename(chunks[0])
                    logging.debug('copy %s/%s to %s' % (source_folder, pattern, install_directory_chunk))

                    # create directory if not exists
                    utils.trymkdir(install_directory_chunk)

                    p = pipeline.make_pipe()
                    # begin
                    if len(chunks) == 3:
                        p = pipeline.find(source_folder, 99)(p)
                    else:
                        p = pipeline.find(source_folder, 0)(p)
                    p = pipeline.grep_basename(pattern)(p)
                    p = pipeline.copy(source_folder, install_directory_chunk)(p)
                    p = pipeline.debug('copied ')(p)
                    # end
                    pipeline.end_pipe()(p)
                logging.debug('end post-install')

        if parameters.fast:
            logging.debug('skipping for because is in fast mode: "compilation"')
            break

    # finish well
    return True
