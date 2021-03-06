#!/bin/sh
# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.

# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.


# Generate tags or cscope files
# Usage tags.sh <mode>
#
# mode may be any of: tags, TAGS, cscope
#
# Uses the following environment variables:
# ARCH, SUBARCH, srctree, src, obj

if [ "$KBUILD_VERBOSE" = "1" ]; then
	set -x
fi

# This is a duplicate of RCS_FIND_IGNORE without escaped '()'
ignore="( -name SCCS -o -name BitKeeper -o -name .svn -o \
          -name CVS  -o -name .pc       -o -name .hg  -o \
          -name .git )                                   \
          -prune -o"

# Do not use full path is we do not use O=.. builds
if [ "${KBUILD_SRC}" = "" ]; then
	tree=
else
	tree=${srctree}/
fi

# Detect if ALLSOURCE_ARCHS is set. If not, we assume SRCARCH
if [ "${ALLSOURCE_ARCHS}" = "" ]; then
	ALLSOURCE_ARCHS=${SRCARCH}
fi

# find sources in arch/$ARCH
find_arch_sources()
{
	find ${tree}arch/$1 $ignore -name "$2" -print;
}

# find sources in arch/$1/include
find_arch_include_sources()
{
	find ${tree}arch/$1/include $ignore -name "$2" -print;
}

# find sources in include/
find_include_sources()
{
	find ${tree}include $ignore -name config -prune -o -name "$1" -print;
}

# find sources in rest of tree
# we could benefit from a list of dirs to search in here
find_other_sources()
{
	find ${tree}* $ignore \
	     \( -name include -o -name arch -o -name '.tmp_*' \) -prune -o \
	       -name "$1" -print;
}

find_sources()
{
	find_arch_sources $1 "$2"
}

all_sources()
{
	for arch in $ALLSOURCE_ARCHS
	do
		find_sources $arch '*.[chS]'
	done
	if [ ! -z "$archinclude" ]; then
		find_arch_include_sources $archinclude '*.[chS]'
	fi
	find_include_sources '*.[chS]'
	find_other_sources '*.[chS]'
}

all_kconfigs()
{
	for arch in $ALLSOURCE_ARCHS; do
		find_sources $arch 'Kconfig*'
	done
	find_other_sources 'Kconfig*'
}

all_defconfigs()
{
	find_sources $ALLSOURCE_ARCHS "defconfig"
}

docscope()
{
	(echo \-k; echo \-q; all_sources) > cscope.files
	cscope -b -f cscope.out
}

exuberant()
{
	all_sources | xargs $1 -a                               \
	-I __initdata,__exitdata,__acquires,__releases          \
	-I __read_mostly,____cacheline_aligned                  \
	-I ____cacheline_aligned_in_smp                         \
	-I ____cacheline_internodealigned_in_smp                \
	-I EXPORT_SYMBOL,EXPORT_SYMBOL_GPL                      \
	--extra=+f --c-kinds=+px                                \
	--regex-asm='/^ENTRY\(([^)]*)\).*/\1/'                  \
	--regex-c='/^SYSCALL_DEFINE[[:digit:]]?\(([^,)]*).*/sys_\1/'

	all_kconfigs | xargs $1 -a                              \
	--langdef=kconfig --language-force=kconfig              \
	--regex-kconfig='/^[[:blank:]]*(menu|)config[[:blank:]]+([[:alnum:]_]+)/\2/'

	all_kconfigs | xargs $1 -a                              \
	--langdef=kconfig --language-force=kconfig              \
	--regex-kconfig='/^[[:blank:]]*(menu|)config[[:blank:]]+([[:alnum:]_]+)/CONFIG_\2/'

	all_defconfigs | xargs -r $1 -a                         \
	--langdef=dotconfig --language-force=dotconfig          \
	--regex-dotconfig='/^#?[[:blank:]]*(CONFIG_[[:alnum:]_]+)/\1/'

}

emacs()
{
	all_sources | xargs $1 -a                               \
	--regex='/^ENTRY(\([^)]*\)).*/\1/'                      \
	--regex='/^SYSCALL_DEFINE[0-9]?(\([^,)]*\).*/sys_\1/'

	all_kconfigs | xargs $1 -a                              \
	--regex='/^[ \t]*\(\(menu\)*config\)[ \t]+\([a-zA-Z0-9_]+\)/\3/'

	all_kconfigs | xargs $1 -a                              \
	--regex='/^[ \t]*\(\(menu\)*config\)[ \t]+\([a-zA-Z0-9_]+\)/CONFIG_\3/'

	all_defconfigs | xargs -r $1 -a                         \
	--regex='/^#?[ \t]?\(CONFIG_[a-zA-Z0-9_]+\)/\1/'
}

xtags()
{
	if $1 --version 2>&1 | grep -iq exuberant; then
		exuberant $1
	elif $1 --version 2>&1 | grep -iq emacs; then
		emacs $1
	else
		all_sources | xargs $1 -a
        fi
}


# Support um (which uses SUBARCH)
if [ "${ARCH}" = "um" ]; then
	if [ "$SUBARCH" = "i386" ]; then
		archinclude=x86
	elif [ "$SUBARCH" = "x86_64" ]; then
		archinclude=x86
	else
		archinclude=${SUBARCH}
	fi
fi

case "$1" in
	"cscope")
		docscope
		;;

	"tags")
		xtags ctags
		;;

	"TAGS")
		xtags etags
		;;
esac
