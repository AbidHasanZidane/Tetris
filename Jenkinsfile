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
            }
        }

        stage('Configure') {
            steps {
                sh 'cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF'
            }
        }

        stage('Build') {
            steps {
                sh 'cmake --build build --parallel'
            }
        }

    }

    post {
        success {
            echo 'Build succeeded.'
        }
        failure {
            echo 'Build failed.'
        }
    }
}
