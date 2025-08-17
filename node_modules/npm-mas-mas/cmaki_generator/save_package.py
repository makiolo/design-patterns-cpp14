import os
import sys
import logging
import argparse
import urllib
import csv
import utils
import subprocess

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--name', required=True, dest='name', help='name package', default=None)
    parser.add_argument('--version', required=True, dest='version', help='version package fixed', default=None)
    parser.add_argument('--depends', required=True, dest='depends', help='json for save versions', default=None)
    parameters = parser.parse_args()

    depends_file = parameters.depends
    if os.path.exists(depends_file):
        data = utils.deserialize(depends_file)
    else:
        data = {}
    # serialize if is new data
    if parameters.name not in data:
        data[parameters.name] = parameters.version
    logging.info('serialize data = %s' % data)
    depends_file_tmp = depends_file + '.tmp'
    utils.serialize(data, depends_file_tmp)
    ret = subprocess.call('python -m json.tool %s > %s' % (depends_file_tmp, depends_file), shell=True)
    os.remove(depends_file_tmp)
    sys.exit(ret)

