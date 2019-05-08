from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'XX-LABEL-XX'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True
config.JobType.numCores = 1
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'XX-CONFIG-XX'
config.JobType.outputFiles = ['Output.root']
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles  = ['Summer16_03Feb2017_V9_MC.db']
#config.JobType.maxMemoryMB = 3500
config.Data.inputDataset = 'XX-DATASET-XX'
#config.Data.userInputFiles= ['/store/mc/RunIIFall17MiniAOD/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/94X_mc2017_realistic_v10-v1/20000/00108AFB-75FB-E711-A917-0025905B85A0.root']
config.Data.inputDBS = 'global'
config.Data.splitting = 'EventAwareLumiBased'
#config.Data.splitting = 'FileBased'
#config.Data.unitsPerJob = 1
#config.Data.unitsPerJob = 2000
config.Data.unitsPerJob = 20
#config.Data.outputPrimaryDataset = 'QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8'
config.Data.outLFNDirBase = '/store/user/rasharma/FirstStep/XX-LABEL-XX/'
config.Data.publication = False
config.Data.outputDatasetTag = 'CRAB3'
#config.Site.whitelist= ['T2_CH_CERN']
config.Site.storageSite = 'T3_US_FNALLPC'

