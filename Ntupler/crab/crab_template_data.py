from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'XX-LABEL-XX'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'XX-CONFIG-XX'
config.JobType.outputFiles = ['Output.root']
config.JobType.allowUndistributedCMSSW = True
config.JobType.numCores = 8
config.JobType.maxMemoryMB = 4500
config.JobType.priority = 99999
config.Data.inputDataset = 'XX-DATASET-XX'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 2
config.Data.lumiMask = 'Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt'
config.Data.runRange = 'XX-RANGE-XX'
config.Data.outLFNDirBase = '/store/user/cmantill/XX-LABEL-XX/'
config.Data.publication = True
config.Data.outputDatasetTag = 'CRAB3'
config.Site.storageSite = 'T3_US_FNALLPC'
