{
  'targets' : [
        #native
        {
            'target_name' : 'node_native',
            'type' : 'static_library',
            'dependencies': [
                '../deps/libuv/uv.gyp:libuv',
                '../deps/http-parser/http_parser.gyp:http_parser'
            ],
            'include_dirs' : [
                '../deps/libuv/include',
                '../deps/http-parser',
                '../deps/http/include'
            ],
            'sources' : [
                '../deps/http/src/loop.cc',
                '../deps/http/src/stream.cc',
                '../deps/http/src/handle.cc',
                '../deps/http/src/net.cc',
                '../deps/http/src/tcp.cc',
                '../deps/http/src/http.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/http/include',
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
                }]
            ]
        }
    ]
}

