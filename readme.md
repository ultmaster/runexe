# runexe

Modified from the runexe from Saratov SU.

They have provided an exe in the polygon package. So this repository will be modified for *NIX only.

Here is my version. (I purpose this will be everything that I will use in hypercube.)

```
This program runs other program(s) for given period of time with specified
restrictions.

Command line format:
  runexe [<global options>] [<process properties>] program [<parameters>]

Global options:
  -help         - show help
  -xml          - print result in xml format (otherwise, use human-readable)
  -interactor="<process properties> interactor <parameters>"
                  INTERACTOR MODE
    Launch another process and cross-connect its stdin&stdout with the main
    program. Inside this flag, you can specify any process-controlling flags:
    interactor can have its own limits, credentials, environment, directory.
    In interactor mode, however, -i and -o have no effects on both main
    program and interactor.
  -ri=<f>       - in interactor mode, record program input to file <f>.
  -ro=<f>       - in interactor mode, record program output to file <f>.

Process properties:
  -t <value>    - time limit. Terminate after <value> seconds, you can use
                  suffix ms to switch to milliseconds. Suffix "s" (seconds)
                  can be omitted.
  -m <value>    - memory limit. Terminate if anonymous virtual memory of the
                  process exceeds <value> bytes. Use suffixes K, M, G to
                  specify kilo, mega, gigabytes.
  -d <value>    - current directory for the process.
  -i <filename> - redirect standard input to <filename>.
  -o <filename> - redirect standard output to <filename>.
  -e <filename> - redirect standard error to <filename>.
```


## Reference

The following command line format is copied from the Windows version.

```
runexe 2.0 version c5a5c20c3bc925a2bbb96cab39d7c21c871d2960 build 138

This program runs other program(s) for given period of time with specified
restrictions.

Command line format:
  runexe [<global options>] [<process properties>] program [<parameters>]

Global options:
  -help         - show help
  -xml          - print result in xml format (otherwise, use human-readable)
  -show-kernel-mode-time - include kernel-mode time in human-readable format
                  (always included in xml)
  -x            - return process exit code (not implemented)
  -logfile=<f>  - for runexe developers only
  -interactor="<process properties> interactor <parameters>"
                  INTERACTOR MODE
    Launch another process and cross-connect its stdin&stdout with the main
    program. Inside this flag, you can specify any process-controlling flags:
    interactor can have its own limits, credentials, environment, directory.
    In interactor mode, however, -i and -o have no effects on both main
    program and interactor.
  -ri=<f>       - in interactor mode, record program input to file <f>.
  -ro=<f>       - in interactor mode, record program output to file <f>.

Process properties:
  -t <value>    - time limit. Terminate after <value> seconds, you can use
                  suffix ms to switch to milliseconds. Suffix "s" (seconds)
                  can be omitted.
  -m <value>    - memory limit. Terminate if anonymous virtual memory of the
                  process exceeds <value> bytes. Use suffixes K, M, G to
                  specify kilo, mega, gigabytes.
  -D k=v        - environment. If any is specified, existing environment is
                  cleared.
  -d <value>    - current directory for the process.
  -l <value>    - login name. Create process under <value> user.
  -p <value>    - password for user specified in -l. On linux, ignored (but
                  must be present).
  -j <filename> - inject <filename> DLL into process.
  -i <filename> - redirect standard input to <filename>.
  -o <filename> - redirect standard output to <filename>.
  -e <filename> - redirect standard error to <filename>.
  -z            - run process in trusted mode.
  -no-idleness-check - switch off idleness checking.
```

And here is an example of verbose output and xml output.

```
Program successfully terminated
  exit code:    0
  time consumed: 0.02 sec
  time passed:  0.17 sec
  peak memory:  6963200 bytes

Interactor successfully terminated
  exit code:    0
  time consumed: 0.00 sec
  time passed:  0.70 sec
  peak memory:  27480064 bytes
```

```
<?xml version="1.1" encoding="UTF-8"?>
<invocationResults>
<invocationResult id="program">
<invocationVerdict>SUCCEEDED</invocationVerdict>
<exitCode>0</exitCode>
<processorUserModeTime>15</processorUserModeTime>
<processorKernelModeTime>62</processorKernelModeTime>
<passedTime>176</passedTime>
<consumedMemory>6909952</consumedMemory>
</invocationResult>
<invocationResult id="interactor">
<invocationVerdict>SUCCEEDED</invocationVerdict>
<exitCode>0</exitCode>
<processorUserModeTime>0</processorUserModeTime>
<processorKernelModeTime>31</processorKernelModeTime>
<passedTime>695</passedTime>
<consumedMemory>27402240</consumedMemory>
</invocationResult>
</invocationResults>
```