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

    $properties['created'] = array(
      'label' => t("Date created"),
      'type' => 'date',
      'schema field' => 'created',
      'description' => t("The date the donation was created."),
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['updated'] = array(
      'label' => t("Date updated"),
      'type' => 'date',
      'schema field' => 'updated',
      'description' => t("The date the donation was most recently updated."),
      'setter callback' => 'entity_property_verbatim_set',
    );

    // Entities
    $properties['entity'] = array(
      'label' => t("Host entity"),
      'type' => 'entity',
      'schema field' => 'entity',
      'description' => t("The host entity."),
      'getter callback' => 'redhen_donation_property_host_get',
      'setter callback' => 'redhen_donation_property_host_set',
    );

    $properties['user'] = array(
      'label' => t("User entity"),
      'type' => 'entity',
      'schema field' => 'uid',
      'description' => t("The entity for which the donation belongs to. Usually a user."),
      'getter callback' => 'redhen_donation_property_user_get',
      'setter callback' => 'redhen_donation_property_user_set',
    );


    return $info;
  }

}
