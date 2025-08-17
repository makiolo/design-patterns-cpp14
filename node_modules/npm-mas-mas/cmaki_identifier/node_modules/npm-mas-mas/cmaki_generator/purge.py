import os
import utils
import logging
from third_party import platforms

def purge(node, parameters):

    package = node.get_package_name()

    logging.debug("Cleaning headers and cmakefiles %s" % package)
    node.remove_scripts_headers()
    node.remove_cmakefiles()

    logging.debug("Cleaning download %s" % package)
    uncompress_directory = node.get_download_directory()
    utils.tryremove_dir(uncompress_directory)

    original_directory = node.get_original_directory()
    utils.tryremove_dir(original_directory)

    for plat in platforms:

        if not node.get_exclude_from_clean():
            logging.debug("Cleaning install %s" % package)
            utils.tryremove_dir(node.get_install_directory(plat))

        build_modes = node.get_build_modes()
        for build_mode in build_modes:

            logging.debug("Cleaning build %s" % package)
            build_directory = node.get_build_directory(plat, build_mode)
            utils.tryremove_dir(build_directory)

    # finish well
    return True

