# Indicate what region to deploy the resources into
generate "provider" {
  path = "provider.tf"
  if_exists = "overwrite_terragrunt"
  contents = <<EOF
  
provider "aws" {
  region = "ap-northeast-1"
 }
 EOF
}

# Backend Bucket
generate "backend" {
  path = "backend.tf"
  if_exists = "overwrite_terragrunt"
  contents = <<EOF
terraform {
  backend "s3" {
    bucket         = "telematics-backend"
    key            = "${path_relative_to_include()}/terraform.tfstate"
    region         = "ap-northeast-1"
  }
}
 EOF
} 
