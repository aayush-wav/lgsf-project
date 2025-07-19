TEMPLATE = subdirs

SUBDIRS += \
    front-end/login \
    front-end/signup \
    front-end/chatbot

# Set login as the default project and main entry point
CONFIG += ordered
SUBDIRS = front-end/login front-end/signup front-end/chatbot

# Make login the primary project
login.file = front-end/login/login.pro

# Include signup project
signup.file = front-end/signup/SIGNUP.pro
signup.depends = login

# Include chatbot project
chatbot.file = front-end/chatbot/chatbot.pro
chatbot.depends = login

# Default target should be login
DEFAULT_PROJECT = front-end/login
