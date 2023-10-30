include "root" {
  path = find_in_parent_folders("terragrunt.hcl")
}

terraform {
  #source = "git@github.com:Deloitte/PES-HMI-Terraform.git//modules/iam?ref=master"
  #source = "/home/ubuntu/actions-runner/_work/PES-HMI-Terraform/PES-HMI-Terraform/modules//iam"
  source  = "/home/runner/work/Telematics-Rpi/Telematics-Rpi/modules//iam"
}

# Indicate the input values to use for the variables of the module.
inputs = {
      name               = "SonarQube"
      POD                = "Telematics"
}
