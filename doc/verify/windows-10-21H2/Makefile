# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

.PHONY: apply ansible destroy

SHELL=/bin/bash

all: apply

terraform.tfvars:
	cat terraform.tfvars.template | sed -e "s/%password%/$$(pwgen -s --remove-chars="\'\"$$%{}" -y 20 1)/gi" > terraform.tfvars

CHROME_WEBEXT_VERSION=$(shell cat ../../../webextensions/chrome/manifest.json | jq --raw-output ".version")
EDGE_WEBEXT_VERSION=$(shell cat ../../../webextensions/edge/manifest.json | jq --raw-output ".version")
manifest.xml:
	cat ../manifest.xml | sed -e "s/CHROME_WEBEXT_VERSION/$(CHROME_WEBEXT_VERSION)/" \
	                          -e "s/EDGE_WEBEXT_VERSION/$(EDGE_WEBEXT_VERSION)/" > ansible/files/manifest.xml

apply: terraform.tfvars manifest.xml
	terraform init
	terraform plan
	time (terraform apply -auto-approve && sleep 10 && ansible-playbook -i ansible/hosts ansible/playbook.yml)

apply-playbook:
	ansible-playbook -i ansible/hosts ansible/playbook.yml

destroy:
	terraform destroy -auto-approve

clean:
	rm -rf terraform.tfvars .terraform terraform.tfstate terraform.tfstate.backup

