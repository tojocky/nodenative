{
  'targets' : [
        #native
        {
            'target_name' : 'node_native',
            'type' : 'static_library',
            'nnative_use_openssl%': 'true',
            'nnative_shared_openssl%': 'false',
            'nnative_target_type%': 'static_library',
            'dependencies': [
                '../deps/libuv/uv.gyp:libuv',
                '../deps/http-parser/http_parser.gyp:http_parser',
            ],
            'include_dirs' : [
                '../deps/libuv/include',
                '../deps/http-parser',
                '../include'
            ],
            'sources' : [
                '../src/loop.cc',
                '../src/stream.cc',
                '../src/handle.cc',
                '../src/net.cc',
                '../src/tcp.cc',
                '../src/http.cc',
                '../src/http/Server.cc',
                '../src/crypto/utils.cc',
                '../src/crypto/PBKDF2.cc',
                '../src/crypto.cc',
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../include',
                    '../deps/libuv/include',
                    '../deps/http-parser'
                ]
            },
            'all_dependent_settings' : {
                'cflags':[
                    '-std=c++1y'
                ]
            },
            'cflags':[
                '-std=c++1y'
            ],
            'conditions' : [
                ['OS=="mac"', {
                    'xcode_settings': {
                        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++1y', '-stdlib=libc++'],
                        #'OTHER_LDFLAGS': ['-stdlib=libc++']
                        #'ARCHS': '$(ARCHS_STANDARD_64_BIT)'
                    },
                    'link_settings': {
                        'libraries': [
                            '$(SDKROOT)/System/Library/Frameworks/CoreServices.framework',
                            '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework'
                        ]
                    },
                    'cflags': [
                        '-stdlib=libc++'
                    ],
                    'all_dependent_settings': {
                        'xcode_settings': {
                            'OTHER_CPLUSPLUSFLAGS' : ['-std=c++1y', '-stdlib=libc++'],
                            #'OTHER_LDFLAGS': ['-stdlib=libc++']
                            #'ARCHS': '$(ARCHS_STANDARD_64_BIT)'
                        },
                        'link_settings': {
                            'libraries': [
                                '$(SDKROOT)/System/Library/Frameworks/CoreServices.framework',
                                '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework'
                            ]
                        },
                        'cflags': [
                            '-stdlib=libc++'
                        ]
                    }
                }],
                ['nnative_use_openssl=="true"', {
                    'defines': [ 'HAVE_OPENSSL=1' ],
                    'sources': [
                    ],
                    'conditions': [
                        [ 'nnative_shared_openssl=="false"', {
                            'dependencies': [
                                '../deps/openssl/openssl.gyp:openssl',

                                # for tests
                                '../deps/openssl/openssl.gyp:openssl-cli',
                            ],
                            # Do not let unused OpenSSL symbols to slip away
                            'conditions': [
                                # -force_load or --whole-archive are not applicable for
                                # the static library
                                [ 'nnative_target_type!="static_library"', {
                                    'xcode_settings': {
                                        'OTHER_LDFLAGS': [
                                            '-Wl,-force_load,<(PRODUCT_DIR)/<(OPENSSL_PRODUCT)',
                                        ]
                                    },
                                    'conditions': [
                                        ['OS in "linux freebsd"', {
                                            'ldflags': [
                                                '-Wl,--whole-archive <(PRODUCT_DIR)/<(OPENSSL_PRODUCT)',
                                                '-Wl,--no-whole-archive',
                                            ]
                                        }]
                                    ]
                                }]
                            ]
                        }]
                    ]
                },
                {
                    'defines': [ 'HAVE_OPENSSL=0' ]
                }]
            ]
        }
    ]
}

