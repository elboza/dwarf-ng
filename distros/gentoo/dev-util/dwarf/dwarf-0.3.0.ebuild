# Copyright 1999-2011 Gentoo Foundation
# dwarf-ng custom ebuild by xnando
# Distributed under the terms of the GNU General Public License v2
# $Header:$
# $Id: dwarf-0.3.0.ebuild 60 2011-01-24 10:33:01Z xnando $
	
# Please note that this file is still experimental !
#
# if you wonder how to use this ebuild :

# su
# DWARF=dwarf-0.3.0
# wget http://download.savannah.gnu.org/releases/dwarf-ng/${DWARF}.tar.gz -O /usr/portage/distfiles/${DWARF}.tgz
# mkdir -p /usr/local/portage/app-misc/dwarf
# tar xvzf /usr/portage/distfiles/${DWARF}.tgz ${DWARF}/distros/gentoo/dev-util/dwarf/${DWARF}.ebuild
# cd /usr/local/portage/dev-util/dwarf
# ebuild  ${DWARF}.ebuild digest
# echo PORTDIR_OVERLAY=/usr/local/portage >> /etc/make.conf
# emerge dwarf

# but read first any suggestions in the README file shipped with the dwarf package!


DESCRIPTION="dwarf : a little and powerful object file manipulation tool."
HOMEPAGE="http://code.google.com/p/dwarf-ng/"
SRC_URI="http://download.savannah.gnu.org/releases/dwarf-ng/${P}.tar.gz"

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
    CONFIGURE_FLAGS=
	
	econf $CONFIGURE_FLAGS ||  die "econf failed for ${P}"
	emake || die "emake failed for ${P}"
}
	
src_install() {
    emake DESTDIR=${D} prefix=/usr install || die
}

