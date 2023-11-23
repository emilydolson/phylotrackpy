#!/bin/bash

python3 -m piptools compile requirements.in --unsafe-package numpy --unsafe-package setuptools
