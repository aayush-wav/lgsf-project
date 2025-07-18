LGSF — Local Government Service Finder
A Qt-based virtual assistant designed to facilitate easy access to local government services

Project Context
This project is developed as the final assignment for the second semester of the Computer Engineering program. LGSF aims to demonstrate practical skills in software development using Qt and C++ by creating an interactive chatbot that helps users find information about local government services efficiently. The project focuses on implementing natural language processing through keyword-based intent matching, a clean user interface, and smooth user interaction.

Overview
LGSF is an intelligent desktop application that provides users with quick and conversational access to local government services such as licenses, permits, public welfare schemes, and more. Using a JSON-based intent database and a dynamic chat interface, LGSF understands user queries and responds with relevant answers in a friendly and accessible manner.

Features
Keyword-based intent matching: Processes user input to identify intents from predefined patterns.

Interactive chat UI: User messages are displayed on the right, bot replies on the left, both in white text for readability.

Typing animation: Bot responses appear letter-by-letter for a natural conversational feel.

Enter key support: Users can send messages by pressing Enter or clicking the send button.

Configurable intents: Easily update or add intents and responses through an external JSON file.

Technologies Used
Qt Framework (C++): For building the desktop GUI and event-driven application logic.

JSON: To store and manage intents and associated response data.

QTimer: Utilized to create the typing animation effect.

PostgreSQL (future work): Planned integration for backend data management.

Setup & Usage
Clone or download the project repository.

Open the project in Qt Creator.

Ensure the intents JSON file is located at D:/LGSF/back-end/json/responses.json or adjust the file path in the code accordingly.

Build and run the application.

Type your questions in the input field (which shows ask anything as placeholder) and press Enter or click Send.

Interact with the chatbot to get answers related to local government services.

Future Enhancements
Integration with live local government databases and APIs for real-time data.

Advanced natural language processing using machine learning techniques.

Voice input and output capabilities for hands-free interaction.

Multilingual support with focus on Nepali and English languages.

Expansion to mobile and web platforms for broader accessibility.

Conclusion
This project showcases the application of software engineering principles, user interface design, and basic natural language processing to solve real-world problems faced by citizens seeking local government services. It serves as a foundational step toward building more intelligent and accessible service-finder applications.

