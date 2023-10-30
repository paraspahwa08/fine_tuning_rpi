include "root" {
  path = find_in_parent_folders("terragrunt.hcl")
}

terraform {
  #source = "git@github.com:Deloitte/PES-HMI-Terraform.git//modules/security_group?ref=master"
  #source = "/home/ubuntu/actions-runner/_work/PES-HMI-Terraform/PES-HMI-Terraform/modules//security_group"
  source  = "/home/runner/work/Telematics-Rpi/Telematics-Rpi/modules//security_group"
}

# Indicate the input values to use for the variables of the module.
inputs = {
      name           = "SonarQube"
      cidr_blocks    = ["157.35.25.110/32"]
      POD            = "Telematics"
}
