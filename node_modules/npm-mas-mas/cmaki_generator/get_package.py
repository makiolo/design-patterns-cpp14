import os
import sys
import logging
import argparse
import urllib
import csv
import utils

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--name', required=True, dest='name', help='name package', default=None)
    parser.add_argument('--depends', required=True, dest='depends', help='json for save versions', default=None)
    parameters = parser.parse_args()

    depends_file = parameters.depends
    if os.path.exists(depends_file):
        data = utils.deserialize(depends_file)
        # data = utils.deserialize_json(depends_file)
    else:
        data = {}
    if parameters.name in data:
        print (data[parameters.name])
        sys.exit(0)
    else:
        sys.exit(1)

