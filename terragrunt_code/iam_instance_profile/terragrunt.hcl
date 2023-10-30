include "root" {
  path = find_in_parent_folders("terragrunt.hcl")
}

terraform {
  #source = "git@github.com:Deloitte/PES-HMI-Terraform.git//modules/iam_instance_profile?ref=master"
  #source = "/home/ubuntu/actions-runner/_work/PES-HMI-Terraform/PES-HMI-Terraform/modules//iam_instance_profile"
  source = "/home/runner/work/Telematics-Rpi/Telematics-Rpi/modules//iam_instance_profile"
}

dependency "iam_instance_profile" {
    config_path = "../iam"
    mock_outputs = {
      iam_role_name = "sample-role-name"
    }
}

# Indicate the input values to use for the variables of the module.
inputs = {
      name               = "SonarQube"
      iam_role           = dependency.iam_instance_profile.outputs.iam_role_name
}
