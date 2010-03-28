set(cloudsync_SRCS
   cloudsync.cpp
   main.cpp
   cloudsyncview.cpp
 )

kde4_add_ui_files(cloudsync_SRCS cloudsyncview_base.ui prefs_base.ui)

kde4_add_kcfg_files(cloudsync_SRCS settings.kcfgc )

kde4_add_executable(cloudsync ${cloudsync_SRCS})

target_link_libraries(cloudsync ${KDE4_KDEUI_LIBS} )

install(TARGETS cloudsync ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES cloudsync.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES cloudsync.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES cloudsyncui.rc  DESTINATION  ${DATA_INSTALL_DIR}/cloudsync )
