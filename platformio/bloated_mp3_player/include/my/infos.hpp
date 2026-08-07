/*
* +==== BEGIN Bloated MP3 Player =================+
* LOGO:
* .......................
* ...><>.............<><.
* ..><>.><>.......><.<><
* .><>.<></><.><>.<><.<><
* ..><>.><>.......<><.<><
* ...><>.............<><.
* .......................
* /STOP
* PROJECT: Bloated MP3 Player
* FILE: infos.hpp
* CREATION DATE: 03-08-2026
* DESCRIPTION:
* The one true path to every human/person-readable message. This file
* forwards to the language pack selected at build time.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Language-pack selector (english / italian / french).
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once

// Which language? Define exactly one of these in platformio.ini's
// build_flags to switch the whole program's language in one go:
//   -DMY_INFOS_LANG_EN   (English, the default)
//   -DMY_INFOS_LANG_IT   (Italian)
//   -DMY_INFOS_LANG_FR   (French)
// English is the fallback when nothing is defined.
#if defined(MY_INFOS_LANG_IT)
    #include "my/infos/italian.hpp"
#elif defined(MY_INFOS_LANG_FR)
    #include "my/infos/french.hpp"
#else
    #include "my/infos/english.hpp"
#endif