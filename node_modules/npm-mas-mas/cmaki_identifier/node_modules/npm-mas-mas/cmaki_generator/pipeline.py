import os
import sys
import logging
import contextlib
import utils
import shutil
from third_party import exceptions_fail_group
from third_party import exceptions_fail_program
from third_party import FailThirdParty


def make_pipe():
    def process():
        pass
    return process


def end_pipe():
    def process(p):
        _ = list(p)
    return process


def _create():
    b = make_pipe()
    e = yield b
    end_pipe()(e)
    yield


@contextlib.contextmanager
def create():
    c = _create()
    p = next(c)
    yield (p, c)


def feed(packages):
    def process(_):
        for node in packages:
            yield node
    return process


def do(function, force, *args, **kwargs):
    '''
    skeleton gtc stage
    '''
    def process(packages):
        def _process():
            for node in packages:
                try:
                    package = node.get_package_name()
                    version = node.get_version()

                    if not force:
                        # skip process if package came with error
                        if node.ret != 0:
                            logging.info('%s %s error detected: skiping' % (function.__name__, package))
                            continue

                        # skip process if package came interrupted
                        if node.interrupted:
                            logging.info('%s %s error detected: skiping' % (function.__name__, package))
                            continue

                    if function.__name__ != 'purge':
                        logger_function = logging.info
                    else:
                        logger_function = logging.debug

                    logger_function('--------- begin@%s: %s (%s) --------' % (function.__name__, package, version))

                    # process package
                    ret = function(node, *args, **kwargs)
                    logging.debug('%s: return %s' % (function.__name__, ret))
                    if isinstance(ret, bool):
                        if not ret:
                            node.ret += 1
                    elif isinstance(ret, int):
                        # aggregation result
                        node.ret += abs(ret)
                    else:
                        logging.error('%s %s error invalid return: %s' % (function.__name__, package, ret))
                        node.ret += 1

                    logger_function('--------- end@%s: %s (%s) --------' % (function.__name__, package, version))

                    if node.ret != 0:
                        node.fail_stage = function.__name__
                        raise FailThirdParty('[exception] %s fail in stage: %s' % (package, function.__name__))

                except FailThirdParty:
                    logging.error('fatal exception in package %s (%s)' % (package, version))
                    node.ret += 1
                    node.fail_stage = function.__name__
                    raise
                except exceptions_fail_group:
                    logging.error('fatal exception in package %s (%s)' % (package, version))
                    node.ret += 1
                    # add exception for show postponed
                    node.exceptions.append(sys.exc_info())
                    node.fail_stage = function.__name__
                    raise
                except exceptions_fail_program:
                    logging.error('interruption in package %s (%s)' % (package, version))
                    node.ret += 1
                    node.fail_stage = function.__name__
                    node.interrupted = True
                    raise
                except:
                    # excepciones por fallos de programacion
                    logging.error('Postponed exception in package %s (%s)' % (package, version))
                    node.ret += 1
                    node.exceptions.append(sys.exc_info())
                    node.fail_stage = function.__name__
                finally:
                    # send to next step
                    yield node

        for node in _process():
            yield node
    return process

####################### PIPELINE PROOF CONCEPT (UNDER CODE IS NOT USED) ###############


def echo(line):
    def process(_):
        yield line
    return process


def cat():
    def process(p):
        for line in p:
            if(os.path.exists(line)):
                with open(line, 'rt') as f:
                    for line2 in f:
                        yield line2
            else:
                logging.warning('<cat> filename %s not exists' % line)
    return process


def find(folder, level=999):
    def process(_):
        for root, dirs, files in utils.walklevel(folder, level):
                for name in files:
                    yield os.path.join(root, name)
    return process


def grep(pattern):
    def process(p):
        for line in p:
            if line.find(pattern) != -1:
                yield line
    return process


def grep_basename(pattern):
    def process(p):
        p0 = pattern[:1]
        pL = pattern[-1:]
        fixed_pattern = pattern.replace('*', '')
        for line in p:
            if(p0 == '*' and pL != '*'):
                if os.path.basename(line).endswith(fixed_pattern):
                    yield line.replace('\\', '/')
            elif(p0 != '*' and pL == '*'):
                if os.path.basename(line).startswith(fixed_pattern):
                    yield line.replace('\\', '/')
            else:
                if os.path.basename(line).find(fixed_pattern) != -1:
                    yield line.replace('\\', '/')
    return process


def grep_v(pattern):
    def process(p):
        for line in p:
            if line.find(pattern) == -1:
                yield line
    return process


def endswith(pattern):
    def process(p):
        for line in p:
            if line.endswith(pattern):
                yield line
    return process


def copy(rootdir, folder):
    def process(p):
        for line in p:
            relfilename = os.path.relpath(line, rootdir)
            destiny = os.path.join(folder, relfilename)
            destiny_dir = os.path.dirname(destiny)
            utils.trymkdir(destiny_dir)
            shutil.copyfile(line, destiny)
            if not os.path.exists(destiny):
                raise Exception("Not exists %s" % destiny)
            yield destiny
    return process


def startswith(pattern):
    def process(p):
        for line in p:
            if line.startswith(pattern):
                yield line
    return process


def printf(prefix = ''):
    def process(p):
        for line in p:
            print("%s%s" % (prefix, line.rstrip()))
            yield line
    return process


def info(prefix = ''):
    def process(p):
        for line in p:
            logging.info("%s%s" % (prefix, line.rstrip()))
            yield line
    return process


def debug(prefix = ''):
    def process(p):
        for line in p:
            logging.debug("%s%s" % (prefix, line.rstrip()))
            yield line
    return process


def write_file(filename, mode='wt'):
    def process(p):
        content = []
        for line in p:
            content.append(line)
        with open(filename, mode) as f:
            for line in content:
                f.write('%s\n' % line.rstrip())
        for line in content:
            yield line
    return process


def tee(filename):
    def process(p):
        p = printf()(p)
        p = write_file(filename)(p)
        for line in p:
            yield line
    return process


def example_context():
    # using context
    with create() as (p, finisher):
        p = find('.')(p)
        p = endswith('.cpp')(p)
        p = cat()(p)
        p = tee('result.txt')(p)
        # send last part
        finisher.send(p)


def example_simple():
    # not using context
    p = make_pipe()
    # begin
    p = find('.', 2)(p)
    p = endswith('.yml')(p)
    p = grep_v('.build_')(p)
    p = tee('result.txt')(p)
    # end
    end_pipe()(p)

if __name__ == '__main__':
    example_simple()
