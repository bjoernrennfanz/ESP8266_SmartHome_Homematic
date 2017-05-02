# Component makefile for homematic

# Include it as 'homematic/<xxx>.h'
INC_DIRS += $(homematic_ROOT)..

# Args for passing into compile rule generation
homematic_SRC_DIR = $(homematic_ROOT)

$(eval $(call component_compile_rules,homematic))