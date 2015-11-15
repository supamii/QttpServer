{
  'targets' : [
        #native
        {
            'target_name' : 'node_native',
            'type' : 'static_library',
            'dependencies': [
                '../libuv/uv.gyp:libuv',
                '../http-parser/http_parser.gyp:http_parser'
            ],
            'include_dirs' : [
                '../libuv/include',
                '../http-parser',
                '../http/include'
            ],
            'sources' : [
                '../http/src/loop.cc',
                '../http/src/stream.cc',
                '../http/src/handle.cc',
                '../http/src/net.cc',
                '../http/src/tcp.cc',
                '../http/src/http.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../http/include',
                    '../libuv/include',
                    '../http-parser'
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

