
scp *.py julien@minibian:/home/julien
pushd ../esp-01/hello
make clean
pushd swig
make clean
popd
zip -r hello.zip *
scp hello.zip  julien@minibian:/tmp/
popd
