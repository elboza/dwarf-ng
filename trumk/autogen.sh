echo "* aclocal...." &&\
aclocal && \
echo "* autoheader...." &&\
autoheader && \
echo "* autoconf...." &&\
autoconf && \
echo "* automake --add-missing --copy" &&\
automake --add-missing --copy

