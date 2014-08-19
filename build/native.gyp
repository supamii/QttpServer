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
                '../native/include'
            ],
            'sources' : [
                '../native/src/loop.cc',
                '../native/src/stream.cc',
                '../native/src/handle.cc',
                '../native/src/net.cc',
                '../native/src/tcp.cc',
                '../native/src/http.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../native/include',
                    '../libuv/include',
                    '../http-parser'
                ]
            },
            'all_dependent_settings' : {
                'cflags':[
                    '-std=c++11'
                ]
            },
            'cflags':[
                '-std=c++11'
            ],
            'conditions' : [
                ['OS=="mac"', {
                    'xcode_settings': {
                        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
                        'OTHER_LDFLAGS': ['-stdlib=libc++'],
                        'ARCHS': '$(ARCHS_STANDARD_64_BIT)'
                    },
                    'link_settings': {
                        'libraries': [
                            '$(SDKROOT)/System/Library/Frameworks/CoreServices.framework',
                            '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework'
                        ]
                    }
                }]
            ]
        }
    ]
}

