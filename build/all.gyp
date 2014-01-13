{
  'targets' : [
    #server
    { 
      'target_name' : 'webserver',
      'type' : 'executable',
      'dependencies': [
        '../libuv/uv.gyp:libuv',
        '../http-parser/http_parser.gyp:http_parser'
      ],
      'sources' : [
        '../samples/webserver.cpp'
      ],
      'include_dirs' : [
        '../libuv/include',
        '../http-parser',
        '../native'
      ],
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
            ],
          },
        }]
      ]
    },
    #client  
    { 
      'target_name' : 'webclient',
      'type' : 'executable',
      'dependencies': [
        '../libuv/uv.gyp:libuv',
        '../http-parser/http_parser.gyp:http_parser'
      ],
      'sources' : [
        '../samples/webclient.cpp'
      ],
      'include_dirs' : [
        '../libuv/include',
        '../http-parser',
        '../native'
      ],
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
            ],
          },

        }]
      ]
    }
  ]
}
