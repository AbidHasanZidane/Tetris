pipeline {
    agent any

    options {
        skipDefaultCheckout true
    }

    environment {
        QT_QPA_PLATFORM = 'offscreen'
    }

    stages {

        stage('Checkout') {
            steps {
                checkout scm
                sh 'echo "=== git ls-files ===" && git ls-files *.cpp *.h'
                sh 'echo "=== pwd ===" && pwd'
                sh 'echo "=== ls ===" && ls -la'
            }
        }

    }

    post {
        always {
            echo 'Pipeline finished.'
        }
    }
}
