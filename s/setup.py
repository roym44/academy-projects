""" This is the build used to create the *.so file 
that will allow symnmf.py to import symnmfmodule.c """

from setuptools import Extension, setup

module = Extension("symnmf_capi", sources=['symnmf.c', 'symnmfmodule.c'])
setup(name='symnmf_capi',
     version='1.0',
     description='Python wrapper for our symnmf C extension',
     ext_modules=[module])