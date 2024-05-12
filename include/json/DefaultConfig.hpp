#pragma once

#include <string>

static std::string const defaultConfigName = "HitScoreVisualizerConfig (default).json";

static std::string const defaultConfigText = R"(
{
  "defaultConfigVersion": 1,
  "isDefaultConfig": true,
  "judgments": [
    {
      "threshold": 115,
      "text": "<size=150%><u>%s</u></size>",
      "color": [
        1.0,
        1.0,
        1.0,
        1.0
      ]
    },
    {
      "threshold": 110,
      "text": "%B<size=120%>%C%s</u></size>%A",
      "color": [
        0.0,
        0.5,
        1.0,
        1.0
      ]
    },
    {
      "threshold": 105,
      "text": "%B%C%s</u>%A",
      "color": [
        0.0,
        1.0,
        0.0,
        1.0
      ]
    },
    {
      "threshold": 100,
      "text": "%B%C%s</u>%A",
      "color": [
        1.0,
        1.0,
        0.0,
        1.0
      ]
    },
    {
      "threshold": 50,
      "text": "%B<size=80%>%s</size>%A",
      "color": [
        1.0,
        0.0,
        0.0,
        1.0
      ],
      "fade": true
    },
    {
      "threshold": 0,
      "text": "%B<size=80%>%s</size>%A",
      "color": [
        1.0,
        0.0,
        0.0,
        1.0
      ]
    }
  ],
  "chainHeadJudgments": [
    {
      "threshold": 85,
      "text": "<size=150%><u>%s</u></size>",
      "color": [
        1.0,
        1.0,
        1.0,
        1.0
      ]
    },
    {
      "threshold": 80,
      "text": "%B<size=120%>%C%s</u></size>%A",
      "color": [
        0.0,
        0.5,
        1.0,
        1.0
      ]
    },
    {
      "threshold": 75,
      "text": "%B%C%s</u>%A",
      "color": [
        0.0,
        1.0,
        0.0,
        1.0
      ]
    },
    {
      "threshold": 70,
      "text": "%B%C%s</u>%A",
      "color": [
        1.0,
        1.0,
        0.0,
        1.0
      ]
    },
    {
      "threshold": 35,
      "text": "%B<size=80%>%s</size>%A",
      "color": [
        1.0,
        0.0,
        0.0,
        1.0
      ],
      "fade": true
    },
    {
      "threshold": 0,
      "text": "%B<size=80%>%s</size>%A",
      "color": [
        1.0,
        0.0,
        0.0,
        1.0
      ]
    }
  ],
  "chainLinkDisplay": {
    "text": "%s",
    "color": [
      1.0,
      1.0,
      1.0,
      1.0
    ]
  },
  "beforeCutAngleJudgments": [
    {
      "threshold": 70,
      "text": " + "
    },
    {
      "threshold": 0,
      "text": "<color=#ff4f4f> - </color>"
    }
  ],
  "accuracyJudgments": [
    {
      "threshold": 15,
      "text": "<u>"
    },
    {
      "threshold": 0,
      "text": ""
    }
  ],
  "afterCutAngleJudgments": [
    {
      "threshold": 30,
      "text": " + "
    },
    {
      "threshold": 0,
      "text": "<color=#ff4f4f> - </color>"
    }
  ]
}
)";
