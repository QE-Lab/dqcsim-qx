# WORK IN PROGRESS

# DQCsim QX backend

[![PyPi](https://badgen.net/pypi/v/dqcsim-qx)](https://pypi.org/project/dqcsim-qx/)

See also: [DQCsim](https://github.com/mbrobbel/dqcsim) and
[QX](https://github.com/QE-Lab/qx-simulator/).

This repository contains some glue code to use the QX simulator as a DQCsim
backend.

Temporary one-liner for running a basic test (the test unfortunately just
uses the installed QX backend, so you need to build & install first):

    sudo pip3 uninstall dqcsim-qx -y && rm -rf target/python/dist && python3 setup.py build && python3 setup.py bdist_wheel && sudo pip3 install target/python/dist/* && python3 setup.py test
