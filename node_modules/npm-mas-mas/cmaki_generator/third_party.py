import os
import sys
import utils
import logging
import traceback
import datetime
import hash_version
import copy
import fnmatch


class InvalidPlatform(Exception):
    def __init__(self, plat):
        self._plat = plat
    def __str__(self):
        return "Invalid platform detected: %s" % self._plat


class DontExistsFile(Exception):
    def __init__(self, source_filename):
        self._source_filename = source_filename
    def __str__(self):
        return 'Dont exists file %s' % self._source_filename


class FailPrepare(Exception):
    def __init__(self, node):
        self._node = node
    def __str__(self):
        return 'Failing preparing package: %s' % self._node.get_package_name()


class AmbiguationLibs(Exception):
    def __init__(self, kind, package, build_mode):
        self._kind = kind
        self._package = package
        self._build_mode = build_mode
    def __str__(self):
        return "Ambiguation in %s in %s. Mode: %s. Candidates:" % (self._kind, self._package, self._build_mode)


class NotFoundInDataset(Exception):
    def __init__(self, msg):
        self._msg = msg
    def __str__(self):
        return "%s" % self._msg


class FailThirdParty(Exception):
    def __init__(self, msg):
        self._msg = msg
    def __str__(self):
        return "%s" % self._msg


class Found(Exception):
    pass


def prepare_cmakefiles(cmake_files):
    if not os.path.isdir(cmake_files):
        logging.error('Invalid cmake files: {}'.format(camkefiles))


def get_identifier(mode):
    env = os.environ.copy()
    cmaki_pwd = env['CMAKI_PWD']
    if utils.is_windows():
        script_identifier = os.path.join(cmaki_pwd, 'bin', 'cmaki_identifier.exe')
    else:
        script_identifier = os.path.join(cmaki_pwd, 'bin', 'cmaki_identifier.sh')
    if not os.path.isfile(script_identifier):
        raise Exception("there is no {} script".format(script_identifier))
    env['CMAKI_INFO'] = mode
    return list(utils.get_stdout(script_identifier, env=env))[0]


def search_fuzzy(data, fuzzy_key, fallback='default'):
    for key in data:
        if fnmatch.fnmatch(fuzzy_key, key):
            return data[key]
    else:
        if fallback in data:
            return data[fallback]
        else:
            logging.error("not found 'default' platform or %s" % fuzzy_key)
            raise Exception("not found '{}'".format(fuzzy_key))


if 'MODE' not in os.environ:
    logging.warning('Using Debug by default. For explicit use, define environment var MODE')
    os.environ['MODE'] = 'Debug'

if 'CMAKI_INSTALL' not in os.environ:
    logging.warning('Using CMAKI_INSTALL by default. For explicit use, define environment var CMAKI_INSTALL')
    os.environ['CMAKI_INSTALL'] = os.path.join( os.getcwd(), '..', 'cmaki_identifier', 'bin')

if 'CMAKI_PWD' not in os.environ:
    logging.warning('Using CMAKI_PWD by default. For explicit use, define environment var CMAKI_PWD')
    os.environ['CMAKI_PWD'] = os.path.join( os.getcwd(), '..', 'cmaki_identifier')


#
# INMUTABLE GLOBALS
#

CMAKELIB_URL='https://github.com/makiolo/cmaki.git'
prefered = {}
prefered['Debug'] = ['Debug', 'RelWithDebInfo', 'Release']
prefered['RelWithDebInfo'] = ['RelWithDebInfo', 'Release', 'Debug']
prefered['Release'] = ['Release', 'RelWithDebInfo', 'Debug']
magic_invalid_file = '__not_found__'
exceptions_fail_group = (OSError, IOError, )
exceptions_fail_program = (KeyboardInterrupt, )
uncompress_strip_default = '.'
uncompress_prefix_default = '.'
priority_default = 50
build_unittests_foldername = 'unittest'
# detect platform
platform = get_identifier('ALL')
arch = get_identifier('ARCH')
operative_system = get_identifier('OS')
somask_id = operative_system[0]
archs = {platform: arch}
platforms = [platform]
logging.info('Detecting platform from script like: {} / {}'.format(platform, arch))

alias_priority_name = { 10: 'minimal',
                        20: 'tools',
                        30: 'third_party' }
alias_priority_name_inverse = {v: k for k, v in alias_priority_name.items()}


def is_valid(package_name, mask):
    return (mask.find(somask_id) != -1) and (package_name != 'dummy')


def is_blacklisted(blacklist_file, no_blacklist, package_name):
    blacklisted = False
    if os.path.exists(blacklist_file):
        with open(blacklist_file, 'rt') as f:
            for line in f.readlines():
                if line.strip() == package_name:
                    blacklisted = True
                    break
    # --no-blacklist can annular effect of blacklist
    if blacklisted and (package_name in no_blacklist):
        blacklisted = False
    return blacklisted


class ThirdParty:
    def __init__(self, user_parameters, name, parameters):
        self.user_parameters = user_parameters
        self.name = name
        self.parameters = parameters
        self.depends = []
        self.exceptions = []
        self.interrupted = False
        self.ret = 0 # Initial return code
        self.fail_stage = ""
        self.blacklisted = is_blacklisted(self.user_parameters.blacklist, self.user_parameters.no_blacklist, self.get_package_name())
        self.published_invalidation = False


    def __hash__(self):
        return hash((self.get_package_name(), self.get_priority(), self.get_mask()))


    def __eq__(self, other):
        return (self.get_package_name() == other.get_package_name()) and (self.get_priority() == other.get_priority()) and (self.get_mask() == other.get_mask())


    def __ne__(self, other):
        return not self.__eq__(other)


    def __repr__(self):
        return "%s (%s)" % (self.get_package_name(), self.get_mask())


    def __str__(self):
        return "%s (%s)" % (self.get_package_name(), self.get_mask())


    def get_uncompress_strip(self, pos = 0):
        try:
            if isinstance(self.parameters['uncompress_strip'], list):
                return self.parameters['uncompress_strip'][pos]
            else:
                return self.parameters['uncompress_strip']
        except KeyError:
            # default value
            return uncompress_strip_default


    def get_uncompress_prefix(self, pos = 0):
        try:
            if isinstance(self.parameters['uncompress_prefix'], list):
                return self.parameters['uncompress_prefix'][pos]
            else:
                return self.parameters['uncompress_prefix']
        except KeyError:
            # default value
            return uncompress_prefix_default


    def get_uncompress(self, pos = 0):
        try:
            if self.parameters['uncompress'] is not None:
                if isinstance(self.parameters['uncompress'], list):
                    return self.parameters['uncompress'][pos].find(somask_id) != -1
                else:
                    return self.parameters['uncompress'].find(somask_id) != -1
            else:
                return False
        except KeyError:
            # default value
            return True


    def get_depends_raw(self):
        return self.depends


    def get_depends(self):
        try:
            return self.parameters['depends']
        except KeyError:
            # default value
            return None


    def get_generate_custom_script(self, source_dir):
        path_build = self.get_path_custom_script(source_dir, name='.build')
        build_content = self.get_build_script_content()
        if build_content is not None:
            with open(path_build, 'wt') as f:
                f.write(build_content)


    def get_path_custom_script(self, source_folder, name = 'build'):
        if utils.is_windows():
            path_build = os.path.join(source_folder, name + '.cmd')
        else:
            path_build = os.path.join(source_folder, name + '.sh')
        return path_build


    def has_custom_script(self, source_folder):
        script_custom = os.path.exists( self.get_path_custom_script(source_folder) )
        return (self.get_build_script_content() is not None) or script_custom


    def get_build_script_content(self):
        try:
            if not utils.is_windows():
                return self.parameters['build']
            else:
                return self.parameters['build_windows']
        except KeyError:
            # default value
            return None


    def get_source(self):
        try:
            source = self.parameters['source']
            if source is not None:
                if not isinstance(source, list):
                    return [source]
                else:
                    return source
            else:
                return []
        except KeyError:
            # default value
            return []


    def get_source_filename(self, position=0):
        try:
            return self.parameters['source_filename']
        except KeyError:
            # default value
            source = self.get_source()[position]
            filename = source.split('/')[-1]
            return filename


    def get_sources_all(self, position=0):
        try:
            return self.parameters['sources_all']
        except KeyError:
            return False


    def get_before_copy(self):
        try:
            return self.parameters['before_copy']
        except KeyError:
            # default value
            return []


    def get_short_path(self):
        try:
            return self.parameters['short_path']
        except KeyError:
            # default value
            return False


    def has_library(self, platform_info):
        package = self.get_package_name()
        return (('static' in platform_info) and (package != 'dummy')) or (('dynamic' in platform_info) and (package != 'dummy'))


    def needs(self, node):
        if node.is_valid():
            self.depends.append(node)


    def get_package_name(self):
        return self.name


    def get_package_name_norm(self):
        package = self.get_package_name()
        for c in '-\\/:*?"<>|':
            package = package.replace(c, '_')
        return package


    def get_package_name_norm_upper(self):
        package_norm = self.get_package_name_norm()
        return package_norm.upper()


    def set_version(self, newversion):
        self.parameters['version'] = newversion


    def get_version(self):
        try:
            version = self.parameters['version']
            if version is None:
                return '0.0.0.0'
            else:
                return version
        except KeyError:
            if self.get_package_name() != 'dummy':
                raise Exception('[%s] Version is a mandatory field.' % self.get_package_name())


    def get_version_manager(self):
        try:
            version = self.get_version()
            if version == '0.0.0.0':
                return self.parameters['version_manager']
            else:
                # si tiene version -> no usar renombrado git
                return None
        except KeyError:
            return None


    def get_cmake_target(self):
        try:
            return self.parameters['cmake_target']
        except KeyError:
            return 'install'


    def get_post_install(self):
        try:
            return self.parameters['post_install']
        except KeyError:
            return []


    def get_priority(self):
        try:
            return int(self.parameters['priority'])
        except KeyError:
            return priority_default


    def is_packing(self):
        try:
            return self.parameters['packing']
        except KeyError:
            # default value
            return True


    def get_branch(self):
        try:
            return self.parameters['branch']
        except KeyError:
            # default value
            return None


    def get_build_modes(self):
        build_modes = []
        try:
            if 'MODE' in os.environ and (os.environ['MODE'] != 'UNDEFINED'):
                build_modes.append(os.environ['MODE'])
            else:
                mode = self.parameters['mode']
                if mode.find('d') != -1:
                    build_modes.append('Debug')
                if mode.find('i') != -1:
                    build_modes.append('RelWithDebInfo')
                if mode.find('r') != -1:
                    build_modes.append('Release')
        except KeyError:
            # no mode provided
            build_modes.append('Debug')
            build_modes.append('RelWithDebInfo')
            build_modes.append('Release')
        return build_modes


    def get_mask(self):
        try:
            return self.parameters['mask']
        except KeyError:
            return somask_id


    def is_valid(self):
        if self.blacklisted:
            if not self.published_invalidation:
                logging.debug('%s is not built because is blacklisted in %s' % (self.get_package_name(), os.path.basename(self.user_parameters.blacklist)))
                self.published_invalidation = True
            return False
        return is_valid(self.get_package_name(), self.get_mask())


    def resolver(self, resolved, seen):
        seen.append(self)
        for edge in self.depends:
            if edge not in resolved:
                if edge in seen:
                    raise Exception('Circular reference detected: %s and %s' % (self.get_package_name(), edge.name))
                edge.resolver(resolved, seen)
        if self.is_valid():
            resolved.append(self)
        seen.remove(self)


    def get_targets(self):
        try:
            return self.parameters['targets']
        except KeyError:
            # default value
            return []


    def get_exclude_from_all(self):
        try:
            return self.parameters['exclude_from_all']
        except KeyError:
            # default value
            return False


    def get_exclude_from_clean(self):
        try:
            return self.parameters['exclude_from_clean']
        except KeyError:
            # default value
            return False


    def get_unittest(self):
        try:
            return self.parameters['unittest']
        except KeyError:
            # default value
            return None


    def get_cmake_prefix(self):
        try:
            cmake_prefix = self.parameters['cmake_prefix']
            if cmake_prefix.endswith('CMakeLists.txt'):
                return os.path.dirname(cmake_prefix)
            return cmake_prefix
        except KeyError:
            # default value
            return "."


    def get_generator_targets(self, plat, _, compiler_cpp, ext_sta, ext_dyn):

        package = self.get_package_name_norm()

        for targets in self.get_targets():

            for target_name in targets:

                platform_info = None
                platform_extra = None

                target_info = targets[target_name]
                if 'info' in target_info:
                    outputinfo = search_fuzzy(target_info['info'], plat)
                    if outputinfo is not None:
                        platform_info = copy.deepcopy( outputinfo )

                if 'extra' in target_info:
                    outputinfo_extra = search_fuzzy(target_info['extra'], plat)
                    if outputinfo_extra is not None:
                        platform_extra = copy.deepcopy( outputinfo_extra )

                if (platform_info is not None) and (platform_extra is not None):
                    platform_info = utils.smart_merge(platform_info, platform_extra)

                # variables for use in "info" and "extra"
                platform_info = utils.apply_replaces_vars(platform_info, {
                                                                            'TARGET': target_name,
                                                                            'TARGET_UPPER': target_name.upper(),
                                                                            'PACKAGE': package,
                                                                            'PACKAGE_UPPER': package.upper(),
                                                                            'PLATFORM': plat,
                                                                            'COMPILER': os.path.basename(compiler_cpp),
                                                                            'EXT_DYN': ext_dyn,
                                                                            'EXT_STA': ext_sta,
                                                                            'ARCH': archs[plat],
                                                                        })

                if platform_info is None:
                    logging.error('No platform info in package %s, platform %s' % (package, plat))
                    logging.error("%s" % targets)
                    sys.exit(1)

                yield (target_name, platform_info)


    def have_any_in_target(self, plat, key, compiler_replace_maps):
        any_static = False
        for compiler_c, compiler_cpp, _, ext_sta, ext_dyn, _, _ in self.compiler_iterator(plat, compiler_replace_maps):
            for package, platform_info in self.get_generator_targets(plat, compiler_c, compiler_cpp, ext_sta, ext_dyn):
                if key in platform_info:
                    any_static = True
        return any_static


    def get_generate_find_package(self):
        try:
            return self.parameters['generate_find_package']
        except KeyError:
            # default value
            return True


    def compiler_iterator(self, plat, compiler_replace_maps):

        plat_parms = search_fuzzy(self.parameters['platforms'], plat)
        try:
            generator = plat_parms['generator']
        except KeyError:
            generator = None

        try:
            compilers = plat_parms['compiler']
        except KeyError:
            compilers = None

        # resolve map
        compiler_replace_resolved = {}
        for var, value in compiler_replace_maps.items():
            new_value = value
            new_value = new_value.replace('$PLATFORM', plat)
            compiler_replace_resolved[var] = new_value
        compiler_replace_resolved['$ARCH'] = archs[plat]
        compiler_replace_resolved['${ARCH}'] = archs[plat]

        # get compiler info
        compiler = get_identifier('COMPILER')

        ext_dyn = plat_parms['ext_dyn']
        ext_sta = plat_parms['ext_sta']
        if compilers is None:
            compilers = [('%s, %s' % (compiler, compiler))]

        for compiler in compilers:
            compilers_tuple = compiler.split(',')
            assert(len(compilers_tuple) == 2)
            compiler_c = compilers_tuple[0].strip()
            compiler_cpp = compilers_tuple[1].strip()

            compiler_c = utils.apply_replaces(compiler_c, compiler_replace_resolved)
            compiler_cpp = utils.apply_replaces(compiler_cpp, compiler_replace_resolved)

            env_new = {}
            env_modified = os.environ.copy()

            for env_iter in [env_modified, env_new]:

                env_iter['COMPILER'] = str(compiler)
                env_iter['PLATFORM'] = str(plat)
                env_iter['PACKAGE'] = str(self.get_package_name())
                env_iter['VERSION'] = str(self.get_version())
                env_iter['ARCH'] = str(archs[plat])

                try:
                    environment = plat_parms['environment']

                    try:
                        environment_remove = environment['remove']
                        for key, values in  environment_remove.items():
                            try:
                                oldpath = env_iter[key]
                            except KeyError:
                                oldpath = ''
                            uniq_values = set()
                            for v in values:
                                v = utils.apply_replaces(v, compiler_replace_resolved)
                                uniq_values.add(v)
                            for v in uniq_values:
                                oldpath = oldpath.replace(v, '')
                            env_iter[key] = oldpath
                    except KeyError:
                        pass

                    # insert front with seprator = ":"
                    try:
                        environment_push_front = environment['push_front']
                        for key, values in  environment_push_front.items():
                            try:
                                oldpath = env_iter[key]
                            except KeyError:
                                oldpath = ''
                            uniq_values = set()
                            for v in values:
                                v = utils.apply_replaces(v, compiler_replace_resolved)
                                uniq_values.add(v)
                            for v in uniq_values:
                                if len(oldpath) == 0:
                                    separator = ''
                                else:
                                    # -L / -I / -R use space
                                    if v.startswith('-'):
                                        separator = ' '
                                    else:
                                        separator = ':'
                                oldpath = str('%s%s%s' % (v, separator, oldpath))
                            env_iter[key] = oldpath
                    except KeyError:
                        pass

                    # insert back with separator " "
                    try:
                        environment_flags = environment['flags']
                        for key, values in  environment_flags.items():
                            try:
                                oldpath = env_iter[key]
                            except KeyError:
                                oldpath = ''
                            uniq_values = set()
                            for v in values:
                                v = utils.apply_replaces(v, compiler_replace_resolved)
                                uniq_values.add(v)
                            for v in uniq_values:
                                if len(oldpath) == 0:
                                    separator = ''
                                else:
                                    separator = ' '
                                oldpath = str('%s%s%s' % (oldpath, separator, v))
                            env_iter[key] = oldpath
                    except KeyError:
                        pass

                    # insert new environment variables
                    try:
                        environment_assign = environment['assign']
                        for key, value in  environment_assign.items():
                            value = utils.apply_replaces(value, compiler_replace_resolved)
                            env_iter[key] = value
                    except KeyError:
                        pass

                except KeyError:
                    pass

            yield (compiler_c, compiler_cpp, generator, ext_sta, ext_dyn, env_modified, env_new)


    def remove_cmake3p(self, cmake3p_dir):
        package_cmake3p = os.path.join(cmake3p_dir, self.get_base_folder())
        logging.debug('Removing cmake3p %s' % package_cmake3p)
        if os.path.exists(package_cmake3p):
            utils.tryremove_dir(package_cmake3p)
        for dep in self.get_depends_raw():
            dep.remove_cmake3p(cmake3p_dir)


    def get_base_folder(self):
        package = self.get_package_name()
        version = self.get_version()
        return '%s-%s' % (package, version)


    def get_workspace(self, plat):
        package = self.get_package_name()
        version = self.get_version()
        return '%s-%s-%s' % (package, version, plat)


    def get_build_directory(self, plat, build_mode):
        package = self.get_package_name()
        version = self.get_version()
        if not self.get_short_path():
            return '.build_%s-%s-%s_%s' % (package, version, plat, build_mode)
        else:
            return '.bs_%s%s%s%s' % (package[:3], version[-1:], plat, build_mode)

    def get_binary_workspace(self, plat):
        install_directory = os.path.join(self.user_parameters.prefix, self.get_workspace(plat))
        utils.trymkdir(install_directory)
        return install_directory


    def get_install_directory(self, plat):
        install_directory = os.path.join(self.get_binary_workspace(plat), self.get_base_folder(), plat)
        return install_directory


    def get_download_directory(self):
        package = self.get_package_name()
        return '.download_%s' % package


    def get_original_directory(self):
        package = self.get_package_name()
        return '.download_original_%s' % package


    def apply_replace_maps(self, compiler_replace_maps):
        package = self.get_package_name()
        package_norm = self.get_package_name_norm()
        to_package = os.path.abspath(package)
        utils.trymkdir(to_package)
        with utils.working_directory(to_package):
            basedir = os.path.abspath('..')
            compiler_replace_maps['$%s_BASE' % package_norm] = os.path.join(basedir, self.get_workspace('$PLATFORM'), self.get_base_folder())


    def generate_scripts_headers(self, compiler_replace_maps):
        package = self.get_package_name()
        package_norm = self.get_package_name_norm()
        version = self.get_version()
        to_package = os.path.abspath(package)
        utils.trymkdir(to_package)
        with utils.working_directory(to_package):
            basedir = self.user_parameters.prefix
            rootdir = self.user_parameters.rootdir

            # generate find.cmake
            build_directory = self.get_build_directory(r"${CMAKI_PLATFORM}", r"${GLOBAL_BUILD_MODE}")
            with open('find.cmake', 'wt') as f:
                f.write("SET(%s_VERSION %s CACHE STRING \"Last version compiled ${PACKAGE}\" FORCE)\n" % (package_norm, version))
                f.write("file(TO_NATIVE_PATH \"%s/%s-%s-${CMAKI_PLATFORM}/%s-%s/${CMAKI_PLATFORM}/include\" %s_INCLUDE)\n" % (basedir, package, version, package, version, package_norm))
                f.write("file(TO_NATIVE_PATH \"%s/%s-%s-${CMAKI_PLATFORM}/%s-%s/${CMAKI_PLATFORM}\" %s_LIBDIR)\n" % (basedir, package, version, package, version, package_norm))
                f.write("file(TO_NATIVE_PATH \"%s/%s\" %s_BUILD)\n" % (rootdir, build_directory, package_norm))
                f.write("SET(%s_INCLUDE ${%s_INCLUDE} CACHE STRING \"Include dir %s\" FORCE)\n" % (package_norm, package_norm, package))
                f.write("SET(%s_LIBDIR ${%s_LIBDIR} CACHE STRING \"Libs dir %s\" FORCE)\n" % (package_norm, package_norm, package))
                f.write("SET(%s_BUILD ${%s_BUILD} CACHE STRING \"Build dir %s\" FORCE)\n" % (package_norm, package_norm, package))

            # genereate find.script / cmd
            if utils.is_windows():
                build_directory = self.get_build_directory("%PLATFORM%", "%BUILD_MODE%")
                with open('find.cmd', 'wt') as f:
                    f.write("set %s_VERSION=%s\n" % (package_norm, version))
                    f.write("set %s_HOME=%s\%s-%s-%%PLATFORM%%\%s-%s\%%PLATFORM%%\n" % (package_norm, basedir, package, version, package, version))
                    f.write("set %s_BASE=%s\%s-%s-%%PLATFORM%%\%s-%s\n" % (package_norm, basedir, package, version, package, version))
                    f.write("set SELFHOME=%s\%%PACKAGE%%-%%VERSION%%-%%PLATFORM%%\%%PACKAGE%%-%%VERSION%%\%%PLATFORM%%\n" % (basedir))
                    f.write("set SELFBASE=%s\%%PACKAGE%%-%%VERSION%%-%%PLATFORM%%\%%PACKAGE%%-%%VERSION%%\n" % (basedir))
                    f.write("set %s_BUILD=%s\%s\n" % (package_norm, rootdir, build_directory))
                    f.write(r"md %SELFHOME%")
                    f.write("\n")
            else:
                build_directory = self.get_build_directory("${PLATFORM}", "${BUILD_MODE}")
                with open('find.script', 'wt') as f:
                    f.write("#!/bin/bash\n")
                    f.write("%s_VERSION=%s\n" % (package_norm, version))
                    f.write("%s_HOME=%s/%s-%s-$PLATFORM/%s-%s/$PLATFORM\n" % (package_norm, basedir, package, version, package, version))
                    f.write("%s_BASE=%s/%s-%s-$PLATFORM/%s-%s\n" % (package_norm, basedir, package, version, package, version))
                    f.write("SELFHOME=%s/$PACKAGE-$VERSION-$PLATFORM/$PACKAGE-$VERSION/$PLATFORM\n" % (basedir))
                    f.write("SELFBASE=%s/$PACKAGE-$VERSION-$PLATFORM/$PACKAGE-$VERSION\n" % (basedir))
                    f.write("%s_BUILD=%s/%s\n" % (package_norm, rootdir, build_directory))
                    f.write("mkdir -p $SELFHOME\n")


    def remove_cmakefiles(self):
        utils.tryremove('CMakeCache.txt')
        utils.tryremove('cmake_install.cmake')
        utils.tryremove('install_manifest.txt')
        utils.tryremove_dir('CMakeFiles')


    def remove_scripts_headers(self):
        package = self.get_package_name()
        to_package = os.path.abspath(package)
        utils.trymkdir(to_package)
        with utils.working_directory(to_package):
            utils.tryremove('find.cmake')
            utils.tryremove('find.script')
            utils.tryremove('find.cmd')
            utils.tryremove('.build.sh')
            utils.tryremove('.build.cmd')
        utils.tryremove_dir_empty(to_package)


    def generate_3rdpartyversion(self, output_dir):
        package = self.get_package_name()
        package_norm_upper = self.get_package_name_norm_upper()
        version = self.get_version()
        packing = self.is_packing()
        if not packing:
            logging.debug("package %s, don't need 3rdpartyversion" % package)
            return
        thirdparty_path = os.path.join(output_dir, '3rdpartyversions')
        utils.trymkdir(thirdparty_path)
        with utils.working_directory(thirdparty_path):
            with open('%s.cmake' % package, 'wt') as f:
                f.write('SET(%s_REQUIRED_VERSION %s EXACT)\n' % (package_norm_upper, version))


    def _smart_uncompress(self, position, package_file_abs, uncompress_directory, destiny_directory, compiler_replace_maps):
        uncompress = self.get_uncompress(position)
        uncompress_strip = self.get_uncompress_strip(position)
        uncompress_prefix = self.get_uncompress_prefix(position)
        if uncompress:
            if (uncompress_strip == uncompress_strip_default) and (uncompress_prefix == uncompress_prefix_default):
                # case fast (don't need intermediate folder)
                ok = utils.extract_file(package_file_abs, destiny_directory, self.get_first_environment(compiler_replace_maps))
            else:
                source_with_strip = os.path.join(uncompress_directory, uncompress_strip)
                destiny_with_prefix = os.path.join(destiny_directory, uncompress_prefix)
                ok = utils.extract_file(package_file_abs, uncompress_directory, self.get_first_environment(compiler_replace_maps))
                utils.move_folder_recursive(source_with_strip, destiny_with_prefix)
                utils.tryremove_dir(source_with_strip)
            if not ok:
                raise Exception('Invalid uncompressed package %s - %s' % (package, package_file_abs))


    def _prepare_third_party(self, position, url, build_directory, compiler_replace_maps):
        package = self.get_package_name()
        source_filename = self.get_source_filename(position)
        uncompress_strip = self.get_uncompress_strip(position)
        uncompress_prefix = self.get_uncompress_prefix(position)
        uncompress = self.get_uncompress(position)
        uncompress_directory = self.get_download_directory()
        utils.trymkdir(uncompress_directory)

        logging.debug('source_filename = %s' % source_filename)
        logging.debug('uncompress_strip = %s' % uncompress_strip)
        logging.debug('uncompress_prefix = %s' % uncompress_prefix)
        logging.debug('uncompress = %s' % uncompress)

        # resolve url vars
        url = url.replace('$NPP_SERVER', os.environ['NPP_SERVER'])

        # files in svn
        if(url.startswith('svn://')):
            # strip is not implemmented with svn://
            utils.tryremove_dir( build_directory )
            logging.info('Download from svn: %s' % url)
            self.safe_system( 'svn co %s %s' % (url, build_directory), compiler_replace_maps )
            # utils.tryremove_dir( os.path.join(build_directory, '.svn') )

        elif(url.endswith('.git') or (url.find('github') != -1) or (url.find('bitbucket') != -1)) and not ( url.endswith('.zip') or url.endswith('.tar.gz') or url.endswith('.tar.bz2') or url.endswith('.tgz') or url.endswith('.py') ):
            # strip is not implemmented with git://
            utils.tryremove_dir( build_directory )
            logging.info('Download from git: %s' % url)
            branch = self.get_branch()
            extra_cmd = ''
            if branch is not None:
                logging.info('clonning to branch %s' % branch)
                extra_cmd = '%s' % branch
            self.safe_system('git clone %s --depth=200 %s %s' % (extra_cmd, url, build_directory), compiler_replace_maps)
            # self.safe_system('git clone %s %s' % (url, build_directory), compiler_replace_maps)
            with utils.working_directory(build_directory):
                # self.safe_system('git checkout {}'.format(extra_cmd), compiler_replace_maps)
                self.safe_system('git submodule init', compiler_replace_maps)
                self.safe_system('git submodule update', compiler_replace_maps)
            # depends_file = self.user_parameters.depends
            # if depends_file is not None:
            #     with utils.working_directory(build_directory):
            #         # leer el fichero de dependencias
            #         if os.path.exists(depends_file):
            #             data = utils.deserialize(depends_file)
            #         else:
            #             data = {}
            #
            #         # obedecer, si trae algo util
            #         if package in data:
            #             logging.debug('data package version is %s' % data[package])
            #             try:
            #                 git_version = hash_version.to_git_version(build_directory, data[package])
            #                 logging.debug('data package in git version is %s' % git_version)
            #                 logging.debug('updating to revision %s' % git_version)
            #                 self.safe_system('git reset --hard %s' % git_version, compiler_replace_maps)
            #             except AssertionError:
            #                 logging.info('using HEAD')
            #
            #         # actualizar y reescribir
            #         revision = hash_version.get_last_version(build_directory)
            #         assert(len(revision) > 0)
            #         data[package] = revision
            #         utils.serialize(data, depends_file)
            # else:
            #     logging.warning('not found depends file, using newest changeset')

        # file in http
        elif (     url.startswith('http://')
                or url.startswith('https://')
                or url.endswith('.zip')
                or url.endswith('.tar.gz')
                or url.endswith('.tar.bz2')
                or url.endswith('.tgz')
                or url.endswith('.py') ):

            logging.info('Download from url: %s' % url)
            # download to source_filename
            package_file_abs = os.path.join(uncompress_directory, source_filename)
            utils.download_from_url(url, package_file_abs)
            if os.path.isfile(package_file_abs):

                # uncompress in download folder for after generate a patch with all changes
                if not os.path.isdir( self.get_original_directory() ):
                    utils.trymkdir( self.get_original_directory() )
                    logging.debug('preparing original uncompress')
                    # uncompress in original
                    self._smart_uncompress(position, package_file_abs, uncompress_directory, self.get_original_directory(), compiler_replace_maps)
                else:
                    logging.debug('skipping original uncompress (already exists)')

                # uncompress in intermediate build directory
                self._smart_uncompress(position, package_file_abs, uncompress_directory, build_directory, compiler_replace_maps)

            else:
                raise DontExistsFile(source_filename)

        else:
            raise Exception('Invalid source: %s - %s' % (package, url))


    def prepare_third_party(self, build_directory, compiler_replace_maps):
        utils.trymkdir(build_directory)
        package = self.get_package_name()
        version = self.get_version()
        sources_all = self.get_sources_all()
        exceptions = []
        i = 0
        for source_url in self.get_source():
            if (source_url is None) or (len(source_url) <= 0) or (source_url == 'skip'):
                logging.warning('[%s %s] Skipping preparation ...' % (package, version))
            else:
                logging.warning('[%s %s] trying prepare from %s ...' % (package, version, source_url))
                try:
                    self._prepare_third_party(i, source_url, build_directory, compiler_replace_maps)
                    if not sources_all:
                        # sources_all = false ---> any source
                        # sources_all = Trie ----> all source
                        break
                except exceptions_fail_group + exceptions_fail_program:
                    raise
                except:
                    exceptions.append(sys.exc_info())
            i += 1
        if len(exceptions) > 0:
            i = 0
            for exc_type, exc_value, exc_traceback in exceptions:
                print ("---- Exception #%d / %d ----------" % (i+1, len(exceptions)))
                traceback.print_exception(exc_type, exc_value, exc_traceback)
                print ("----------------------------------")
                i += 1
            raise FailPrepare(self)


    def get_prefered_build_mode(self, prefered_build_mode_list):
        build_modes = self.get_build_modes()
        assert(len(prefered_build_mode_list) > 0)
        prefered_build_mode = prefered_build_mode_list[0]
        while (prefered_build_mode not in build_modes) and (len(prefered_build_mode_list)>0):
            prefered_build_mode_list.pop(0)
            if len(prefered_build_mode_list) > 0:
                prefered_build_mode = prefered_build_mode_list[0]
        return prefered_build_mode


    def generate_cmake_condition(self, platforms, compiler_replace_maps):
        target_uniques = set()
        condition = ''
        i = 0
        for plat in platforms:
            for compiler_c, compiler_cpp, _, ext_sta, ext_dyn, _, _ in self.compiler_iterator(plat, compiler_replace_maps):
                for package, platform_info in self.get_generator_targets(plat, compiler_c, compiler_cpp, ext_sta, ext_dyn):
                    package_lower = package.lower()
                    if (package_lower not in target_uniques) and (package_lower != 'dummy'):
                        target_uniques.add(package_lower)
                        if self.has_library(platform_info):
                            if i == 0:
                                condition += '(NOT TARGET %s)' % package_lower
                            else:
                                condition += ' OR (NOT TARGET %s)' % package_lower
                        i += 1
        return condition


    def _search_library(self, rootdir, special_pattern):
        '''
        3 cases:
            string
            pattern as special string
            list of strings
        '''
        logging.debug('-- searching in {} with pattern: {}'.format(rootdir, special_pattern))

        if special_pattern is None:
            logging.debug('Failed searching lib in %s' % rootdir)
            return False, None

        package = self.get_package_name()
        if isinstance(special_pattern, list):
            utils.verbose(self.user_parameters, 'Searching list %s' % special_pattern)
            valid_ff = None
            for ff in special_pattern:
                valid, valid_ff = self._search_library(rootdir, utils.get_norm_path(ff))
                if valid:
                    break
            return valid, valid_ff

        elif special_pattern.startswith('/') and special_pattern.endswith('/'):
            pattern = special_pattern[1:-1]
            utils.verbose(self.user_parameters, 'Searching rootdir %s, pattern %s' % (rootdir, pattern))
            files_found = utils.rec_glob(rootdir, pattern)
            utils.verbose(self.user_parameters, 'Candidates %s' % files_found)
            if len(files_found) == 1:
                relfile = os.path.relpath(files_found[0], rootdir)
                return True, utils.get_norm_path(relfile)
            elif len(files_found) == 0:
                msg = 'No library found in %s with pattern %s' % (rootdir, pattern)
                logging.debug(msg)
                return False, None
            else:
                msg = "Ambiguation in %s" % (package)
                logging.debug(msg)
                return False, None
        else:
            pathfull = os.path.join(rootdir, special_pattern)
            utils.verbose(self.user_parameters, 'Checking file %s' % pathfull)
            if os.path.exists(pathfull):
                return True, utils.get_norm_path(special_pattern)
            else:
                return False, None


    def search_library(self, workbase, dataset, kind, rootdir=None):
        '''
        can throw exception
        '''
        build_mode = self.get_prefered_build_mode(prefered[os.environ['MODE']])
        if rootdir is None:
            rootdir = workbase
        utils.verbose(self.user_parameters, 'Searching rootdir %s' % (rootdir))
        if (build_mode.lower() in dataset) and (kind in dataset[build_mode.lower()]):
            special_pattern = dataset[build_mode.lower()][kind]
            valid, valid_ff = self._search_library(rootdir, special_pattern)
            if valid:
                return valid_ff
            else:
                package = self.get_package_name()
                raise AmbiguationLibs(kind, package, build_mode)
        else:
            raise NotFoundInDataset("Not found in dataset, searching %s - %s" % (build_mode.lower(), kind))


    def search_library_noexcept(self, workbase, dataset, kind):
        try:
            rootdir = os.path.abspath(workbase)
            finalpath = self.search_library(workbase, dataset, kind, rootdir)
            utils.superverbose(self.user_parameters, '[01] path: %s' % finalpath)
            return finalpath
        except AmbiguationLibs:
            finalpath = '%s.%s' % (magic_invalid_file, kind)
            utils.superverbose(self.user_parameters, '[02] path: %s' % finalpath)
            return finalpath
        except NotFoundInDataset:
            finalpath = '%s.%s' % (magic_invalid_file, kind)
            utils.superverbose(self.user_parameters, '[03] path: %s' % finalpath)
            return finalpath


    def check_parts_exists(self, workbase, package, target, dataset, kindlibs, build_modes=None):
        '''
        Asegura que todas las partes del target existen, devuelve True o False si todas las partes existen

        workbase: directorio de instalacion base 
        package: nombre del paquete
        target: nombre del target
        dataset: es la estructura que contiene las estrategias de busqueda
            {"debug": {"part1": ["*.dll", "*d.dll"]}, "release": {"part1": ["*_release.dll"]}}
        kindlibs: tupla de partes a verificar, cada tupla representa (tipo, obligatoriedad)
        build_modes: restringuir la busqueda a ciertos build modes
        '''

        all_ok = True
        if build_modes is None:
            build_modes = self.get_build_modes()
        for build_mode in build_modes:
            for kind, must in kindlibs:
                try:
                    part_fullpath = os.path.join(workbase, self.search_library_noexcept(workbase, dataset, kind))
                    if not os.path.exists(part_fullpath):
                        if must:
                            logging.error("[%s] Don't found %s in %s. Mode: %s. Path: %s. Dataset: %s" % (package, kind, target, build_mode, part_fullpath, dataset))
                            all_ok = False
                        else:
                            msg = "[%s] Don't found %s in %s. Mode: %s. Path: %s" % (package, kind, target, build_mode, part_fullpath)
                            if build_mode != 'Release':
                                logging.warning(msg)
                            else:
                                logging.debug(msg)
                except NotFoundInDataset as e:
                    if must:
                        logging.error("[ERROR] [NOT FOUND] [%s] %s" % (package, e))
                        all_ok = False
        return all_ok


    def is_invalid_lib(self, libpath):
        return (libpath is None) or (utils.get_filename_no_ext(os.path.basename(libpath)) == magic_invalid_file)


    def generate_cmakefiles(self, platforms, folder_output, compiler_replace_maps):
        errors = 0
        packing = self.is_packing()
        if not packing:
            logging.warning("package: %s don't need generate cmakefiles" % self.get_package_name())
            return errors
        oldcwd = os.getcwd()
        utils.trymkdir(folder_output)
        with utils.working_directory(folder_output):
            package = self.get_package_name()
            package_lower = package.lower()
            package_upper = package.upper()
            with open('%s-config.cmake' % package_lower, 'wt') as f:
                f.write('''CMAKE_POLICY(PUSH)
CMAKE_POLICY(VERSION 3.0)
cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0011 NEW)
                ''')

                condition = self.generate_cmake_condition(platforms, compiler_replace_maps)
                if len(condition) > 0:
                    f.write('\nif(%s)\n' % condition)

                f.write('''\ninclude(${CMAKI_PATH}/facts/facts.cmake)
cmaki_download_package()
file(TO_NATIVE_PATH "${_DIR}" %s_HOME)
file(TO_NATIVE_PATH "${_DIR}/${CMAKI_PLATFORM}" %s_PREFIX)
set(%s_HOME "${%s_HOME}" PARENT_SCOPE)
set(%s_PREFIX "${%s_PREFIX}" PARENT_SCOPE)
include(${_MY_DIR}/${CMAKI_PLATFORM}.cmake)
                ''' % (package_upper, package_upper, package_upper, package_upper, package_upper, package_upper))

                if len(condition) > 0:
                    f.write('\nendif()\n')

                f.write('\nCMAKE_POLICY(POP)')

            with open('%s-config-version.cmake' % package_lower, 'wt') as f:
                f.write('''\
cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0011 NEW)
include(${CMAKI_PATH}/facts/facts.cmake)
cmaki_package_version_check()
                ''')

            for plat in platforms:

                workspace = self.get_workspace(plat)
                base_folder = self.get_base_folder()

                for compiler_c, compiler_cpp, _, ext_sta, ext_dyn, env_modified, _ in self.compiler_iterator(plat, compiler_replace_maps):

                    with open('%s.cmake' % (plat), 'wt') as f:

                        install_3rdparty_dependencies = True

                        includes_set = []
                        definitions_set = []
                        system_depends_set = []
                        depends_set = set()

                        for target, platform_info in self.get_generator_targets(plat, compiler_c, compiler_cpp, ext_sta, ext_dyn):

                            target_lower = target.lower()
                            target_upper = target.upper()

                            if self.has_library(platform_info) and (target != 'dummy'):
                                f.write('if(NOT TARGET %s)\n\n' % target_lower)

                            try:
                                add_3rdparty_dependencies = platform_info['add_3rdparty_dependencies']
                            except KeyError:
                                add_3rdparty_dependencies = True

                            try:
                                lib_provided = platform_info['lib_provided']
                            except KeyError:
                                lib_provided = True

                            if 'include' in platform_info:
                                include = platform_info['include']
                                for d in include:
                                    includes_set.append(d)

                            # rename to definitions
                            if 'definitions' in platform_info:
                                definitions = platform_info['definitions']
                                if definitions is not None:
                                    for d in definitions:
                                        definitions_set.append(d)

                            if 'system_depends' in platform_info:
                                system_depends = platform_info['system_depends']
                                if system_depends is not None:
                                    for sd in system_depends:
                                        system_depends_set.append(sd)

                            if 'targets_paths' in self.parameters:
                                targets_paths = self.parameters['targets_paths']
                                if targets_paths is not None:
                                    for key, value in targets_paths.items():
                                        f.write('file(TO_NATIVE_PATH "%s" %s)\n' % (value, key))

                            # work_base = os.path.join(oldcwd, workspace, base_folder, plat)
                            work_base = self.get_install_directory(plat)

                            if ('executable' in platform_info) and (target != 'dummy'):
                                # a target in mode executable, dont need install
                                install_3rdparty_dependencies = False

                                if 'use_run_with_libs' in platform_info:
                                    if utils.is_windows():
                                        f.write('file(TO_NATIVE_PATH "${_MY_DIR}/../../run_with_libs.cmd" %s_LAUNCHER)\n' % target_upper)
                                    else:
                                        f.write('file(TO_NATIVE_PATH "${_MY_DIR}/../../run_with_libs.sh" %s_LAUNCHER)\n' % target_upper)

                                executable = platform_info['executable']
                                if not self.check_parts_exists(work_base, package, target, executable, [('bin', True)], build_modes=['Release']):
                                    errors += 1
                                release_bin = self.search_library_noexcept(work_base, executable, 'bin')

                                for suffix in ['', '_EXECUTABLE']:
                                    if 'use_run_with_libs' in platform_info:
                                        f.write('set(%s%s "${%s_LAUNCHER}" "${_DIR}/%s/%s" PARENT_SCOPE)\n' % (target_upper, suffix, target_upper, plat, utils.get_norm_path(release_bin, native=False)))
                                    else:
                                        f.write('set(%s%s "${_DIR}/%s/%s" PARENT_SCOPE)\n' % (target_upper, suffix, plat, utils.get_norm_path(release_bin, native=False)))
                                    f.write('file(TO_NATIVE_PATH "${%s%s}" %s%s)\n' % (target_upper, suffix, target_upper, suffix))
                                f.write('\n')

                            if ('dynamic' in platform_info) and (target != 'dummy'):

                                dynamic = platform_info['dynamic']

                                # add depend
                                if add_3rdparty_dependencies:
                                    f.write('list(APPEND %s_LIBRARIES %s)\n' % (package_upper, target_lower))

                                if utils.is_windows():
                                    if not self.check_parts_exists(work_base, package, target, dynamic, [('dll', True), ('lib', lib_provided), ('pdb', False)]):
                                        errors += 1

                                    debug_dll = self.search_library_noexcept(work_base, dynamic, 'dll')
                                    release_dll = self.search_library_noexcept(work_base, dynamic, 'dll')
                                    relwithdebinfo_dll = self.search_library_noexcept(work_base, dynamic, 'dll')
                                    minsizerel_dll = self.search_library_noexcept(work_base, dynamic, 'dll')

                                    debug_lib = self.search_library_noexcept(work_base, dynamic, 'lib')
                                    release_lib = self.search_library_noexcept(work_base, dynamic, 'lib')
                                    relwithdebinfo_lib = self.search_library_noexcept(work_base, dynamic, 'lib')
                                    minsizerel_lib = self.search_library_noexcept(work_base, dynamic, 'lib')

                                    try:
                                        relwithdebinfo_pdb = self.search_library(work_base, dynamic, 'pdb')
                                    except Exception as e:
                                        logging.debug('exception searching lib: %s' % e)
                                        relwithdebinfo_pdb = None

                                    try:
                                        debug_pdb = self.search_library(work_base, dynamic, 'pdb')
                                    except Exception as e:
                                        logging.debug('exception searching lib: %s' % e)
                                        debug_pdb = None

                                    f.write('ADD_LIBRARY(%s SHARED IMPORTED)\n' % target_lower)
                                    f.write('SET_PROPERTY(TARGET %s APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)\n' % target_lower)
                                    f.write('SET_TARGET_PROPERTIES(%s PROPERTIES\n' % target_lower)

                                    # dll
                                    f.write('\tIMPORTED_LOCATION_DEBUG "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(debug_dll, native=False)))
                                    f.write('\tIMPORTED_LOCATION_RELEASE "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(release_dll, native=False)))
                                    f.write('\tIMPORTED_LOCATION_RELWITHDEBINFO "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(relwithdebinfo_dll, native=False)))
                                    f.write('\tIMPORTED_LOCATION_MINSIZEREL "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(minsizerel_dll, native=False)))
                                    f.write('\n')

                                    # lib
                                    if not self.is_invalid_lib(debug_lib):
                                        f.write('\tIMPORTED_IMPLIB_DEBUG "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(debug_lib, native=False)))
                                    if not self.is_invalid_lib(release_lib):
                                        f.write('\tIMPORTED_IMPLIB_RELEASE "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(release_lib, native=False)))
                                    if not self.is_invalid_lib(relwithdebinfo_lib):
                                        f.write('\tIMPORTED_IMPLIB_RELWITHDEBINFO "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(relwithdebinfo_lib, native=False)))
                                    if not self.is_invalid_lib(minsizerel_lib):
                                        f.write('\tIMPORTED_IMPLIB_MINSIZEREL "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(minsizerel_lib, native=False)))
                                    f.write('\n')

                                    # pdb
                                    if not self.is_invalid_lib(debug_pdb):
                                        f.write('\tIMPORTED_PDB_DEBUG "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(debug_pdb, native=False)))

                                    if not self.is_invalid_lib(relwithdebinfo_pdb):
                                        f.write('\tIMPORTED_PDB_RELWITHDEBINFO "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(relwithdebinfo_pdb, native=False)))

                                    f.write(')\n')
                                else:

                                    if not self.check_parts_exists(work_base, package, target, dynamic, [('so', True)]):
                                        errors += 1

                                    debug_so = self.search_library_noexcept(work_base, dynamic, 'so')
                                    release_so = self.search_library_noexcept(work_base, dynamic, 'so')
                                    relwithdebinfo_so = self.search_library_noexcept(work_base, dynamic, 'so')
                                    minsizerel_so = self.search_library_noexcept(work_base, dynamic, 'so')

                                    try:
                                        debug_so_full = os.path.join(oldcwd, work_base, debug_so)
                                        debug_soname = utils.get_soname(debug_so_full, env=env_modified)
                                        logging.debug('detected soname in debug library: {}'.format(debug_soname))
                                    except Exception as e:
                                        logging.debug('exception searching lib: %s' % e)
                                        debug_soname = None

                                    try:
                                        release_so_full = os.path.join(oldcwd, work_base, release_so)
                                        release_soname = utils.get_soname(release_so_full, env=env_modified)
                                        logging.debug('detected soname in release library: {}'.format(release_soname))
                                    except Exception as e:
                                        logging.debug('exception searching lib: %s' % e)
                                        release_soname = None

                                    try:
                                        relwithdebinfo_so_full = os.path.join(oldcwd, work_base, relwithdebinfo_so)
                                        relwithdebinfo_soname = utils.get_soname(relwithdebinfo_so_full, env=env_modified)
                                        logging.debug('detected soname in relwithdebinfo library: {}'.format(relwithdebinfo_soname))
                                    except Exception as e:
                                        logging.debug('exception searching lib: %s' % e)
                                        relwithdebinfo_soname = None

                                    try:
                                        minsizerel_so_full = os.path.join(oldcwd, work_base, minsizerel_so)
                                        minsizerel_soname = utils.get_soname(minsizerel_so_full, env=env_modified)
                                        logging.debug('detected soname in minsizerel library: {}'.format(minsizerel_soname))
                                    except Exception as e:
                                        logging.debug('exception searching lib: %s' % e)
                                        minsizerel_soname = None

                                    f.write('ADD_LIBRARY(%s SHARED IMPORTED)\n' % target_lower)
                                    f.write('SET_PROPERTY(TARGET %s APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)\n' % target_lower)
                                    f.write('SET_TARGET_PROPERTIES(%s PROPERTIES\n' % target_lower)

                                    # so
                                    f.write('\tIMPORTED_LOCATION_DEBUG "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(debug_so, native=False)))
                                    f.write('\tIMPORTED_LOCATION_RELEASE "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(release_so, native=False)))
                                    f.write('\tIMPORTED_LOCATION_RELWITHDEBINFO "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(relwithdebinfo_so, native=False)))
                                    f.write('\tIMPORTED_LOCATION_MINSIZEREL "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(minsizerel_so, native=False)))
                                    f.write('\n')

                                    # soname
                                    if (debug_soname is not None) and os.path.exists( os.path.join(os.path.dirname(debug_so_full), debug_soname) ):
                                        f.write('\tIMPORTED_SONAME_DEBUG "%s"\n' % utils.get_norm_path(debug_soname, native=False))

                                    if (release_soname is not None) and os.path.exists( os.path.join(os.path.dirname(release_so_full), release_soname) ):
                                        f.write('\tIMPORTED_SONAME_RELEASE "%s"\n' % utils.get_norm_path(release_soname, native=False))

                                    if (relwithdebinfo_soname is not None) and os.path.exists( os.path.join(os.path.dirname(relwithdebinfo_so_full), relwithdebinfo_soname) ):
                                        f.write('\tIMPORTED_SONAME_RELWITHDEBINFO "%s"\n' % utils.get_norm_path(relwithdebinfo_soname, native=False))

                                    if (minsizerel_soname is not None) and os.path.exists( os.path.join(os.path.dirname(minsizerel_so_full), minsizerel_soname) ):
                                        f.write('\tIMPORTED_SONAME_MINSIZEREL "%s"\n' % utils.get_norm_path(minsizerel_soname, native=False))

                                    f.write(')\n')

                            if ('static' in platform_info) and (target != 'dummy'):

                                static = platform_info['static']

                                if not self.check_parts_exists(work_base, package, target, static, [('lib', True)]):
                                    errors += 1

                                debug_lib = self.search_library_noexcept(work_base, static, 'lib')
                                release_lib = self.search_library_noexcept(work_base, static, 'lib')
                                relwithdebinfo_lib = self.search_library_noexcept(work_base, static, 'lib')
                                minsizerel_lib = self.search_library_noexcept(work_base, static, 'lib')

                                if add_3rdparty_dependencies:
                                    # register target
                                    f.write('list(APPEND %s_LIBRARIES %s)\n' % (package_upper, target_lower))

                                f.write('ADD_LIBRARY(%s STATIC IMPORTED)\n' % target_lower)
                                f.write('SET_PROPERTY(TARGET %s APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)\n' % target_lower)
                                f.write('SET_TARGET_PROPERTIES(%s PROPERTIES\n' % target_lower)

                                # lib
                                f.write('\tIMPORTED_LOCATION_DEBUG "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(debug_lib, native=False)))
                                f.write('\tIMPORTED_LOCATION_RELEASE "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(release_lib, native=False)))
                                f.write('\tIMPORTED_LOCATION_RELWITHDEBINFO "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(relwithdebinfo_lib, native=False)))
                                f.write('\tIMPORTED_LOCATION_MINSIZEREL "${_DIR}/%s/%s"\n' % (plat, utils.get_norm_path(minsizerel_lib, native=False)))

                                f.write(')\n')

                            if install_3rdparty_dependencies and (target != 'dummy'):
                                f.write('cmaki_install_3rdparty(%s)\n' % target_lower)
                            f.write('\n')

                            if self.has_library(platform_info) and (target != 'dummy'):
                                f.write('endif()\n\n')

                        # print includes
                        if len(includes_set) > 0:
                            for d in list(set(includes_set)):
                                f.write('list(APPEND %s_INCLUDE_DIRS ${_DIR}/%s)\n' % (package_upper, d))

                            f.write('\n')

                        if len(definitions_set) > 0:
                            for d in list(set(definitions_set)):
                                f.write('add_definitions(%s)\n' % d)
                            f.write('\n')

                        if len(system_depends_set) > 0:
                            f.write('# begin system depends\n')
                            for sd in list(set(system_depends_set)):
                                f.write('list(APPEND %s_LIBRARIES %s)\n' % (package_upper, sd))
                            f.write('# end system depends\n')

                        # if self.get_generate_find_package():
                        #     f.write('# Depends of %s (%s)\n' % (self.get_package_name(), self.get_version()))
                        #     for dep in self.get_depends_raw():
                        #         package_name = dep.get_package_name()
                        #         if package_name not in depends_set:
                        #             if dep.have_any_in_target(plat, 'dynamic', compiler_replace_maps):
                        #                 f.write('cmaki_find_package(%s)\n' % (package_name))
                        #             else:
                        #                 f.write('# cmaki_find_package(%s) # static package\n' % (package_name))
                        #             depends_set.add(package_name)
                        #     f.write('\n')

                logging.info('----------------------------------------------------')
                if self.user_parameters.fast:
                    logging.debug('skipping for because is in fast mode: "generate_cmakefiles"')
                    break

        return errors


    def show_environment_vars(self, env_modified):
        package = self.get_package_name()
        logging.debug('------- begin print environment variables for compile %s ---------' % package)
        for key, value in sorted(env_modified.items()):
            logging.debug("%s=%s" % (key, value))
        logging.debug('------- end print environment variables for compile %s -----------' % package)


    def get_first_environment(self, compiler_replace_maps):
        for plat in platforms:
            for _, _, _, _, _, env_modified, _ in self.compiler_iterator(plat, compiler_replace_maps):
                return env_modified
        return os.environ.copy()


    def safe_system(self, cmd, compiler_replace_maps):
        return utils.safe_system(cmd, env=self.get_first_environment(compiler_replace_maps))


    def remove_packages(self):
        # remove packages before
        for plat in platforms:
            prefix_package = os.path.join(self.user_parameters.prefix, '%s.tar.gz' % self.get_workspace(plat))
            prefix_package_cmake = os.path.join(self.user_parameters.prefix, '%s-cmakelib-%s.tar.gz' % (self.get_base_folder(), sys.platform))
            prefix_folder_cmake = os.path.join(self.user_parameters.third_party_dir, self.get_base_folder())
            logging.info("preremoving package %s" % prefix_package)
            logging.info("preremoving package cmakefiles %s" % prefix_package_cmake)
            logging.info("preremoving folder cmakefiles %s" % prefix_folder_cmake)
            utils.tryremove(prefix_package)
            utils.tryremove(prefix_package_cmake)
            utils.tryremove_dir(prefix_folder_cmake)


