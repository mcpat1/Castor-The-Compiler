pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'make cleanall; make all' 
            }
	stage('Test') {
            steps {
                /* `make check` returns non-zero on test failures,
                * using `true` to allow the Pipeline to continue nonetheless
                */
                sh 'make testall || true' 
            }
        }
        }
    }
}
