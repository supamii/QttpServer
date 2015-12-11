{
  'targets' : [
        #native
        {
            'target_name' : 'node_native',
            'type' : 'static_library',
            'dependencies': [
                '../lib/libuv/uv.gyp:libuv',
                '../lib/http-parser/http_parser.gyp:http_parser'
            ],
            'include_dirs' : [
                '../lib/libuv/include',
                '../lib/http-parser',
                '../lib/http/include'
            ],
            'sources' : [
                '../lib/http/src/loop.cc',
                '../lib/http/src/stream.cc',
                '../lib/http/src/handle.cc',
				'../lib/http/src/fs.cc',
                '../lib/http/src/net.cc',
                '../lib/http/src/tcp.cc',
                '../lib/http/src/http.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../lib/http/include',
                    '../lib/libuv/include',
                    '../lib/http-parser'
                ]
            },
            'all_dependent_settings' : {
                'cflags':[
                    '-std=c++0x'
                ]
            },
            'cflags':[
                '-std=c++0x'
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

