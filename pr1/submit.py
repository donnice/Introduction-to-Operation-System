import time
import os
import sys
import argparse
import json
import datetime
from bonnie.submission import Submission


def main():
    parser = argparse.ArgumentParser(description='Submits code to the Udacity site.')
    parser.add_argument('quiz', choices=['echo', 'transfer', 'gfclient', 'gfserver', 'gfclient_mt', 'gfserver_mt', 'readme'])
    parser.add_argument('--provider', choices=['gt', 'udacity'], default='gt')
    parser.add_argument('--environment', choices=['local', 'development', 'staging', 'production'],
                        default='production')

    args = parser.parse_args()

    path_map = {'echo': 'echo',
                'transfer': 'transfer',
                'gfclient': 'gflib',
                'gfserver': 'gflib',
                'gfclient_mt': 'mtgf',
                'gfserver_mt': 'mtgf',
                'readme': '.'}

    quiz_map = {'echo': 'pr1_echo_client_server',
                'transfer': 'pr1_transfer',
                'gfclient': 'pr1_gfclient',
                'gfserver': 'pr1_gfserver',
                'gfclient_mt': 'pr1_gfclient_mt',
                'gfserver_mt': 'pr1_gfserver_mt',
                'readme': 'pr1_readme'}

    files_map = {'pr1_echo_client_server': ['echoclient.c', 'echoserver.c'],
                 'pr1_transfer': ['transferclient.c', 'transferserver.c'],
                 'pr1_gfclient': ['gfclient.c'],
                 'pr1_gfserver': ['gfserver.c'],
                 'pr1_gfclient_mt': ['gfclient_download.c'],
                 'pr1_gfserver_mt': ['gfserver_main.c', 'handler.c'],
                 'pr1_readme': ['readme-student.md']}

    quiz = quiz_map[args.quiz]

    os.chdir(path_map[args.quiz])

    submission = Submission('cs8803-02', quiz,
                            filenames=files_map[quiz],
                            environment=args.environment,
                            provider=args.provider)

    timestamp = "{:-%Y-%m-%d-%H-%M-%S}".format(datetime.datetime.now())

    while not submission.poll():
        time.sleep(3.0)

    if submission.result():
        result = submission.result()

        filename = "%s-result%s.json" % (args.quiz, timestamp)

        with open(filename, "w") as fd:
            json.dump(result, fd, indent=4, separators=(',', ': '))

        for t in result['tests']:
            description = '{:70s}'.format(t['description'][:69] + ":")
            passfail = t['output']['passfail']
            print '%s %s' % (description, passfail.rjust(9))

        print "(Details available in %s.)" % os.path.join(path_map[args.quiz], filename)

    elif submission.error_report():
        error_report = submission.error_report()

        filename = "%s-error-report%s.json" % (args.quiz, timestamp)

        with open(filename, "w") as fd:
            json.dump(error_report, fd, indent=4, separators=(',', ': '))

        print "Something went wrong.  Please see the error report in %s." % os.path.join(path_map[args.quiz], filename)

    else:
        print "Unknown error."


if __name__ == '__main__':
    main()
