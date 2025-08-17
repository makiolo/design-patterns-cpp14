import logging


def set_state(rets, key, value):
    if not key in rets:
        rets[key] = value
    else:
        logging.warning('Received in pipeline multiples packages with same name and version: %s' % key)
        set_state(rets, key + '_', value)


def get_return_code(parameters, rets):
    def process(packages):
        for node in packages:
            try:
                # process package
                name = node.get_package_name()
                version = node.get_version()

                if len(node.exceptions) > 0:
                    state = "EXCEPTION in %s" % node.fail_stage
                elif node.interrupted:
                    state = "INTERRUPTED in %s" % node.fail_stage
                elif (node.ret != 0):
                    state = "FAILED in %s" % node.fail_stage
                else:
                    state = "OK"

                key = '%s - %s' % (name, version)
                set_state(rets, key, state)
            finally:
                # send to next step
                yield node
    return process

