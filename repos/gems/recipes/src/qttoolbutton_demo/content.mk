MIRROR_FROM_REP_DIR := src/app/qttoolbutton_demo

MIRROR_FROM_LIBPORTS := src/app/qt5/tmpl/target_defaults.inc \
                        src/app/qt5/tmpl/target_final.inc

content: $(MIRROR_FROM_REP_DIR) $(MIRROR_FROM_LIBPORTS) LICENSE

$(MIRROR_FROM_REP_DIR):
	$(mirror_from_rep_dir)

LIBPORT_DIR := $(GENODE_DIR)/repos/libports

$(MIRROR_FROM_LIBPORTS):
	mkdir -p $(dir $@)
	cp -r $(LIBPORT_DIR)/$@ $(dir $@)

LICENSE:
	cp $(GENODE_DIR)/LICENSE $@
