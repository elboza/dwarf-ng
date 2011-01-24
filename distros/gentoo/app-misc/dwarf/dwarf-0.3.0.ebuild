# Copyright 1999-2007 Gentoo Foundation
# dwarf-ng custom ebuild by xnando
# Distributed under the terms of the GNU General Public License v2

# $Id: dwarf-0.3.0.ebuild 60 2011-01-24 10:33:01Z xnando $
	
# Please note that this file is still experimental !
#
# if you wonder how to use this ebuild :

# su
# DWARF=dwarf-0.3.0
# wget http://download.savannah.gnu.org/releases/dwarf-ng/${DWARF}.tar.gz -O /usr/portage/distfiles/${DWARF}.tgz
# mkdir -p /usr/local/portage/app-misc/dwarf
# tar xvzf   /usr/portage/distfiles/${DWARF}.tar.gz ${DWARF}/distros/gentoo/app-misc/dwarf/${DWARF}.ebuild
# cd /usr/local/portage/app-misc/dwarf
# ebuild  ${DWARF}.ebuild digest
# echo PORTDIR_OVERLAY=/usr/local/portage >> /etc/make.conf
# emerge dwarf

# but read first any suggestions in the README file shipped with the dwarf package!




# the description
DESCRIPTION="dwarf : a little and powerful object file manipulation tool."

# the official web page
HOMEPAGE="http://code.google.com/p/dwarf-ng/"

# the official mirror
SRC_URI="http://download.savannah.gnu.org/releases/dwarf-ng/${P}.tar.gz"

# for testing
#SRC_URI="ftp://127.0.0.1/${P}.tar.gz"
#SRC_URI="http://code.autistici.org/svn/dwarf"


LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86 ppc ~ppc64 amd64 ~ia64"
IUSE=""

DEPEND="sys-libs/readline
        sys-devel/flex
        sys-devel/bison"

RDEPEND=""

src_unpack() {
        unpack ${A}
        cd ${S}
}

src_compile() {
        # Let autoconf do its job and then fix things to build fbida
        # according to our specifications

        CONFIGURE_FLAGS=
	
	econf $CONFIGURE_FLAGS ||  die "econf failed for ${P}"
	emake || die "emake failed for ${P}"
	}
	
	src_install() {
	# this is wrong : gonna discover why
	#       make install || die
	
	# this is good : gonna discover why
	        make \
	                DESTDIR=${D} \
	                prefix=/usr \
	                install || die
	
	#       if ! use pdf; then
	#               rm -f ${D}/usr/bin/fimgs ${D}/usr/share/man/man1/fimgs.1
	#       fi
	}
	
	# more docs for lamers :P :
	# www.gentoo.org/devel/handbook.xml
	# http://linuxreviews.org/gentoo/ebuilds
	# man 5 ebuild
	#
	
