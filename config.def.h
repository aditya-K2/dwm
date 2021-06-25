/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>
#include "layouts.c"
#include "fibonacci.c"
/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 2;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 1;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 4;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int vertpad            = 0;       /* vertical padding of bar */
static const int sidepad            = 0;      /* horizontal padding of bar */
static const char *fonts[]          = { "SourceCodeProMedium:size=9" , "NotoColorEmoji:size=10"};
static const char dmenufont[]       = "SourceCodeProMedium:size=10";
static const char col_gray3[]       = "#bbbbbb";
static const char col_statusbarBG[] = "#000f1a";
static const char col_statusbarMiddle[] = "#212121";
static const char col_selectedBorder[] = "#212121";
static const char col_notselectedBorder[] = "#000000";
static const char col_statusbarFG[] = "#ffffff";
static const char col_dmenuFG[] = "#bbbbbb"; 
static const char *colors[][3]      = { 
	/*               fg         bg         border   */ 
	[SchemeNorm] = { col_gray3, col_statusbarBG, col_notselectedBorder},
	[SchemeSel]  = { col_statusbarFG, col_statusbarMiddle,  col_selectedBorder},
	[SchemeHid]  = { col_gray3,  col_statusbarBG, col_notselectedBorder  },
};

/* tagging */
static const char *tags[] = {"", "", "", "", "", "", "", "", "" };
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class               instance    title       tags mask   isfloating   monitor */
	{ "Gimp",		     	NULL,       NULL,      1<<4,       0,           -1 },
	{ "Inkscape",		    NULL,       NULL,      1<<4,       0,           -1 },
	{ "Firefox",			NULL,       NULL,      3,          0,           -1 },
	{ "TelegramDesktop",    NULL,       NULL,      1<<2,       0,           -1 },
	{ "qBittorrent",        NULL,       NULL,      1<<3,       0,           -1 },
	{ "Spotify",		    NULL,       NULL,      1<<5,       0,           -1 },
	{ "zoom",				NULL,       NULL,      1<<6,       0,           -1 },
	{ "figma-linux",	 	NULL,       NULL,      1<<4,       0,           -1 },
	{ "discord",	 	    NULL,       NULL,      1<<7,       0,           -1 },
	{ "VSCodium",	 	    NULL,       NULL,      1<<2,       0,           -1 },
	{ "Steam",	 	        NULL,       NULL,      1<<3,       0,           -1 },
	{ "Tk",	 	            NULL,       NULL,      0,          1,           -1 },
	{ "Pavucontrol",	    NULL,       NULL,      1<<8,       0,           -1 },
	{ "Pulseeffects",	    NULL,       NULL,      1<<8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "|  ",      tile },    /* first entry is default */
	{ "|  ",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "HHH",      grid },
 	{ "[@]",      spiral },
 	{ "[\\]",      dwindle },
	{ "TTT",      bstack },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ Mod1Mask,                     KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,		    KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-p", ">", "-m", dmenumon, "-fn", dmenufont, "-nb", col_statusbarBG, "-nf", col_dmenuFG, "-sb", col_statusbarMiddle, "-sf", col_statusbarFG, "-i" , NULL };
static const char *termcmd[]  = { "/usr/bin/alacritty", NULL };
static const char *web[] = { "/usr/bin/brave", NULL };
static const char *torrent[] = { "/usr/bin/qbittorrent", NULL };
static const char *fileManager[] = { "/usr/bin/nautilus", NULL };
/* static const char *vscodium[] = { "/usr/bin/vscodium", NULL }; */
static const char scratchpadname[] = "scratchpad";
static const char ncmpcppname[] = "ncmpcpp";
static const char *scratchpadcmd[] = { "alacritty", "-t", scratchpadname, NULL };
static const char *ncmpcppcmd[] = { "alacritty", "-t", ncmpcppname, "-e", ncmpcppname,  NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,						XK_u,	   spawn,          {.v = web} },
	{ MODKEY,						XK_q,	   spawn,          {.v = torrent} },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_n,      spawn,		   {.v = fileManager } },
	{ MODKEY,                       XK_space,  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_y,  togglencmpcpp,  {.v = ncmpcppcmd } },
	{ Mod1Mask,                     XK_F4,     spawn,          SHCMD("sd") }, //add to path
	{ 0,                            XK_Print,  spawn,          SHCMD("screenshot") }, 
	{ 0,                            XK_F8,     spawn,          SHCMD("feh --bg-scale --randomize /D/Downloads/*.jpg") }, 
	{ ControlMask,                  XK_Print,  spawn,          SHCMD("directoryName") }, 
	{ MODKEY|ShiftMask,             XK_y,      spawn,          SHCMD("LD_PRELOAD=/home/aditya/suckless/spotifywm/spotifywm.so /home/aditya/suckless/scripts/spotadfree") }, 
	{ 0,                            XF86XK_AudioLowerVolume, spawn, SHCMD("pamixer -d 5 ; pkill -RTMIN+10 dwmblocks")}, //XK COMMAND TO DECREASE VOLUME
	{ 0,                            XF86XK_AudioMute, spawn, SHCMD("pulsemute")}, //XK COMMAND TO MUTE VOLUME
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, SHCMD("pamixer -i 5 ; pkill -RTMIN+10 dwmblocks")},	//XK COMMAND TO INCREASE VOLUME
	{ 0,                            XF86XK_AudioPrev, spawn, SHCMD("playerctlPrev")},	//XK COMMAND TO previous song 
	{ 0,                            XF86XK_AudioNext, spawn, SHCMD("playerctlNext")},	//XK COMMAND TO next song
	{ 0,                            XF86XK_AudioPlay, spawn, SHCMD("playerctlToggle")},	//XK COMMAND TO next song
	{ 0,                            XF86XK_AudioStop, spawn, SHCMD("mpc stop")},	//XK COMMAND TO next song
	{ MODKEY,                       XK_F11, spawn, SHCMD("pamixer -d 5 ; pkill -RTMIN+10 dwmblocks")}, //ALTERNATE CONTROLS
	{ MODKEY,                       XK_F12, spawn, SHCMD("pamixer -i 5 ; pkill -RTMIN+10 dwmblocks")}, //ALTERNATE CONTROLS
	{ MODKEY|ShiftMask,             XK_F5,  spawn, SHCMD("reloadKeys")}, // map keys again  
	{ MODKEY,                       XK_F6,  spawn, SHCMD("feh --randomize --bg-scale /D/Downloads/wallpapers/*.jpg")}, // map keys again  
	{ MODKEY|ShiftMask,             XK_F6,  spawn, SHCMD("sxiv /D/Downloads/wallpapers/*.jpg")}, // map keys again  
	{ MODKEY,                       XK_F10,    spawn,          SHCMD("xbacklight -inc 10 ; pkill -RTMIN+20 dwmblocks")},
	{ MODKEY,                       XK_F9,     spawn,          SHCMD("xbacklight -dec 10 ; pkill -RTMIN+20 dwmblocks")},
	{ MODKEY,                       XK_e,      spawn,          SHCMD("getEmoji")},
	{ MODKEY,                       XK_w,      spawn,          SHCMD("alacritty -e neomutt")},
	{ MODKEY|ShiftMask,             XK_o,      spawn,          SHCMD("scriptMaker")},
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_r,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY|ShiftMask,             XK_r,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       XK_F2,      spawn,          SHCMD("cph")},
	{ MODKEY,                       XK_z,      spawn,          SHCMD("unzipFiles")},
	{ MODKEY,                       XK_bracketright,      spawn,          SHCMD("alacritty -e fs")},
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_j,      rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      rotatestack,    {.i = -1 } },
	{ MODKEY,                       XK_j,      focusstackvis,  {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstackvis,  {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      focusstackhid,  {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      focusstackhid,  {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_minus,  incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,						XK_m,	   zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_c,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
	{ MODKEY|ShiftMask,             XK_u,      setlayout,      {.v = &layouts[6]} },
	{ MODKEY,                       XK_s,      show,           {0} },
	{ MODKEY,                       XK_t,      hide,           {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask|ControlMask, XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
