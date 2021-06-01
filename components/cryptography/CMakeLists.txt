idf_component_register(SRCS "cryptography.c"
                    INCLUDE_DIRS "."
                     REQUIRES "util_uart" "mbedtls")