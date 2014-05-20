<?php

/**
 * @see hook_entity_property_info()
 */
class RedhenDonationMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['donation_id'] = array(
      'label' => t("Donation ID"),
      'type' => 'integer',
      'description' => t("The unique ID of the donation."),
      'schema field' => 'donation_id',
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['type'] = array(
      'label' => t("Donation type"),
      'type' => 'token',
      'description' => t("The type of the donation."),
      'options list' => 'donation_type_get_names',
      'required' => TRUE,
      'schema field' => 'type',
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['entity_type'] = array(
      'label' => t("Host entity type"),
      'type' => 'token',
      'description' => t("The entity type of the host entity."),
      'required' => TRUE,
      'schema field' => 'entity_type',
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['entity_id'] = array(
      'label' => t("Host entity ID"),
      'type' => 'integer',
      'description' => t("The entity ID of the host entity."),
      'required' => TRUE,
      'schema field' => 'entity_id',
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['pledged'] = array(
      'label' => t("Amount pledged."),
      'type' => 'integer',
      'description' => t("Amount pledged."),
      'schema field' => 'pledged',
      'setter callback' => 'entity_property_verbatim_set',
      'getter callback' => 'redhen_donation_property_pledged_get',
    );

    $properties['received'] = array(
      'label' => t("Amount received."),
      'type' => 'integer',
      'description' => t("Amount received."),
      'schema field' => 'received',
      'setter callback' => 'entity_property_verbatim_set',
      'getter callback' => 'redhen_donation_property_received_get'
    );

    $properties['created'] = array(
      'label' => t("Date created"),
      'type' => 'date',
      'schema field' => 'created',
      'description' => t("The date the donation was created."),
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['status'] = array(
      'label' => t("Status"),
      'type' => 'int',
      'schema field' => 'status',
      'description' => t("The status."),
      'getter callback' => 'redhen_donation_property_status_get',
    );

    $properties['updated'] = array(
      'label' => t("Date updated"),
      'type' => 'date',
      'schema field' => 'updated',
      'description' => t("The date the donation was most recently updated."),
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['entity'] = array(
      'label' => t("Host entity"),
      'type' => 'entity',
      'schema field' => 'entity',
      'description' => t("The host entity."),
      'getter callback' => 'redhen_donation_property_host_get',
      'setter callback' => 'redhen_donation_property_host_set',
    );

    $properties['author'] = array(
      'label' => t("Author user entity"),
      'type' => 'entity',
      'schema field' => 'author_uid',
      'description' => t("The entity for which the donation belongs to. Usually a user."),
      'getter callback' => 'redhen_donation_property_user_get',
      'setter callback' => 'redhen_donation_property_user_set',
    );

    $properties['transaction_entity'] = array(
      'type' => 'entity',
      'getter callback' => 'redhen_donation_property_transation_entity_get',
      'description' => t("The order or recurring entity associated with this donation."),
    );

    $properties['contact'] = array(
      'type' => 'redhen_contact',
      'schema field' => 'contact_id',
      'getter callback' => 'redhen_donation_property_redhen_contact_get',
      'description' => t("The RedHen contact associated with this donation."),
    );

    return $info;
  }

}
