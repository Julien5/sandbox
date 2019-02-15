rm -vf /tmp/*.zip
zip -r /tmp/server.zip *.sh *.py html
scp /tmp/server.zip julien@minibian:/tmp/
pushd ../esp-01/hello
make clean
pushd swig
make clean
popd
zip -r /tmp/swig.zip *
scp /tmp/swig.zip  julien@minibian:/tmp/
popd
scp install_server.sh julien@minibian:/tmp/
ssh julien@minibian "/tmp/install_server.sh"
