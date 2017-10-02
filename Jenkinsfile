node {
  checkout([$class: 'GitSCM', branches: [[name: '*/master']], browser: [$class: 'GithubWeb', repoUrl: 'https://github.com/makiolo/design-patterns-cpp14'], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'CleanBeforeCheckout']], submoduleCfg: [], userRemoteConfigs: [[url: 'https://github.com/makiolo/design-patterns-cpp14.git']]])
  sh "IMAGE=linux-x64 npm run docker"
}
