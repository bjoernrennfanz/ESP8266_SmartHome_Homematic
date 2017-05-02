# Component makefile for tado_v2_api

# Include it as 'tado_v2_api/<xxx>.h'
INC_DIRS += $(tado_v2_api_ROOT)..

# Args for passing into compile rule generation
tado_v2_api_SRC_DIR = $(tado_v2_api_ROOT)

$(eval $(call component_compile_rules,tado_v2_api))