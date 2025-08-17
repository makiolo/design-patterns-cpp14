import os
import contextlib
import utils
import time
from datetime import datetime
from utils import get_stdout
from email.utils import parsedate


def get_revision_svn(repo):
    '''
    This command need svn in PATH
    '''
    cmd = "svn info %s" % repo
    for line in get_stdout(cmd):
        if line.startswith('Last') or (line.startswith('Revisi') and (line.find('cambio') != -1)):
            pos = line.rindex(':')
            return int(line[pos+2:])
    return -1


def get_timestamp_from_changeset(repo, changeset_searched):
    '''
    generator of commits
    '''
    with utils.working_directory(repo):
        lines = []
        for line in get_stdout(r'git log --format="%H;%cd" --date=rfc'):
            lines.append(line)
        for line in reversed(lines):
            chunks = line.split(";")
            assert(len(chunks) == 2)
            changeset = chunks[0]
            timestamp = int(time.mktime(parsedate(chunks[1])))
            if changeset_searched == changeset:
                return timestamp
    raise Exception('Error in get timestamp from changeset {}'.format(changeset_searched))


def git_log_gen(repo, number=1, extra=''):
    '''
    generator of commits
    '''
    with utils.working_directory(repo):
        for line in get_stdout('git log -%d %s' % (number, extra)):
            if line.startswith('commit'):
                parts = line.split(' ')
                assert(len(parts) == 2)
                commit_name = parts[1]
                yield commit_name


def get_changeset_git_from_position(repo, position = 0):
    with utils.working_directory(repo):
        i = 1
        lines = []
        for line in get_stdout('git log'):
            lines.append(line)
        for line in reversed(lines):
            if line.startswith('commit'):
                parts = line.split(' ')
                assert(len(parts) == 2)
                commit_name = parts[1]
                if i == position:
                    return commit_name
                else:
                    i += 1
    raise Exception('Error in get git hash from position {}'.format(position))


def get_changeset_from_timestamp(repo, timestamp_searched):
    with utils.working_directory(repo):
        lines = []
        for line in get_stdout(r'git log --format="%H;%cd" --date=rfc'):
            lines.append(line)
        for line in reversed(lines):
            chunks = line.split(";")
            assert(len(chunks) == 2)
            changeset = chunks[0]
            timestamp =  int(time.mktime(parsedate(chunks[1])))
            if timestamp_searched == timestamp:
                return changeset
    raise Exception('Error in get git hash from timestamp {}'.format(timestamp_searched))


def get_position_git_from_changeset(repo, changeset):
    with working_directory(repo):
        i = 1
        lines = []
        for line in get_stdout('git log'):
            lines.append(line)
        for line in reversed(lines):
            if line.startswith('commit'):
                parts = line.split(' ')
                if len(parts) == 2:
                    commit_name = parts[1]
                    if commit_name == changeset:
                        return i
                    else:
                        i += 1
    return -1


def get_last_changeset(repo, short=False):
    for changeset in git_log_gen(repo, number=1):
        if short:
            return changeset[:7]
        else:
            return changeset
    return ""


def get_last_version(repo):
    return to_cmaki_version(repo, get_last_changeset(repo))


def rehash_simple(commit_name, position):
    separator = '000'
    return int(separator.join(list(str(ord(character)) for character in commit_name))) % position


@contextlib.contextmanager
def working_directory(path):
    prev_cwd = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev_cwd)


def to_cmaki_version(repo, changeset):
    '''
    git hash ----> 0.0.x.x
    '''
    position = get_timestamp_from_changeset(repo, changeset)
    hash_simple = rehash_simple(changeset, position)
    versions = []
    versions.append('0')
    versions.append('0')
    versions.append(str(position))
    versions.append(str(hash_simple))
    return '.'.join(versions)


def to_git_version(repo, version):
    '''
    0.0.x.x ----> git hash
    '''
    version = version.split('.')
    assert(len(version) == 4)
    position = int(version[2])
    pseudohash = int(version[3])
    changeset = get_changeset_from_timestamp(repo, position)
    hash_simple = rehash_simple(changeset, position)
    assert( get_timestamp_from_changeset(repo, changeset) == position )
    assert( hash_simple == pseudohash )
    return changeset


if __name__ == '__main__':

    local_path = r'/home/ricardo/dev/fast-event-system'

    for commit_name in git_log_gen(local_path, 10):
        cmaki_version = to_cmaki_version(local_path, commit_name)
        print ("%s -> %s" % (commit_name, cmaki_version))
        commit_name2 = to_git_version(local_path, cmaki_version)
        print ("%s -> %s" % (cmaki_version, commit_name2))
        print ()


