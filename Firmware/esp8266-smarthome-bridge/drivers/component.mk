# Component makefile for drivers

# Include it as 'drivers/<xxx>.h'
INC_DIRS += $(drivers_ROOT)..

# Args for passing into compile rule generation
drivers_SRC_DIR = $(drivers_ROOT)

$(eval $(call component_compile_rules,drivers))