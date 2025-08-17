import os
import logging
import utils
from third_party import platforms


def upload(node, parameters, compiler_replace_maps):

    if parameters.server is None:
        logging.warning('parameter --server is mandatory for upload, skipping upload')
    else:
        # pack tar.gz binaries
        for plat in platforms:
            prefix_package = os.path.join(parameters.prefix, '%s.tar.gz' % node.get_workspace(plat))
            if not os.path.isfile(prefix_package):
                logging.error('error dont exitsts: {}'.format(prefix_package))
                return False
            command = "python upload_package.py --url=%s/upload.php --filename=%s" % (parameters.server, prefix_package)
            node.ret += abs(utils.safe_system(command))

        if node.ret != 0:
            return False

        # pack cmakefiles
        if not parameters.no_packing_cmakefiles:
            for plat in platforms:
                base_folder = node.get_base_folder()
                prefix_package_cmake = os.path.join(parameters.prefix, '%s-%s-cmake.tar.gz' % (base_folder, plat))
                if not os.path.isfile(prefix_package_cmake):
                    logging.error('error dont exitsts: {}'.format(prefix_package_cmake))
                    return False
                command = "python upload_package.py --url=%s/upload.php --filename=%s" % (parameters.server, prefix_package_cmake)
                node.ret += abs(utils.safe_system(command))

    return True
