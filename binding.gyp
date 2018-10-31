{
  "targets": [
    {
      "target_name": "volume_controller",
      "sources": [ "src/volume_controller.cc" ],
      "libraries": [ "'<!@(pkg-config alsa --libs)'"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}