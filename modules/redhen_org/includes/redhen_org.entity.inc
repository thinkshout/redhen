<?php
/**
 * @file
 * Redhen Group entity classes
 */

/**
 * The class used for group entities.
 */
class RedhenOrg extends Entity {

  public $primary_contact;

  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_org');

    $this->primary_contact = $this->primaryContact();
  }

  /**
   * Override buildContent() to add organization properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content['label'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Label'),
      '#items' => array(
        array(
          '#markup' => check_plain($this->label)
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );
    $content['redhen_state'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('State'),
      '#items' => array(
        array(
          '#markup' => ($this->redhen_state == REDHEN_STATE_ACTIVE) ? t('Active') : t('Inactive')
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Set the primary contact for an organization
   *
   * @param $contact
   *  The redhen_contact.contact_id to use as the primary contact
   *
   * @return bool
   *  Return FALSE if we are unable to set the primary contact
   */
  public function setPrimary($contact) {
    $return = TRUE;
    //@TODO check that $contact has a relationship with $this
    //
    $this->primary_contact_id = $contact->contact_id;
    $this->save();

    return $return;
  }

  /**
   * Set the redhen_state for an organization
   *
   * @param $state
   *  The REDHEN_STATE_* value to use
   */
  public function setState($state) {
    $this->redhen_state = $state;
    $this->save();
  }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $type = redhen_org_get_types($this->type);
    return $type->label . ': ' . $this->label;
  }

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    return array('path' => 'redhen/org/' . $this->identifier());
  }

  /**
   * Return a primary contact object if possible.
   *
   * @return RedhenContact|null
   */
  protected function primaryContact() {
    $contact = NULL;
    if (isset($this->primary_contact_id)) {
      $contact = redhen_contact_load($this->primary_contact_id);
    }
    return $contact;
  }

}

/**
 * Controls metadata for Redhen organizations
 */
class RedhenOrgMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['created'] = array(
      'label' => t("Created"),
      'description' => t("The date the organization was created."),
      'type' => 'date',
      'schema field' => 'created',
    );
    $properties['updated'] = array(
      'label' => t("Updated"),
      'description' => t("The date the organization was updated."),
      'type' => 'date',
      'schema field' => 'updated',
    );
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the organization record."),
      'schema field' => 'author_uid'
    );
    $properties['primary_contact'] = array(
      'label' => t("Primary contact"),
      'type' => 'redhen_contact',
      'description' => t("The primary contact for the organization."),
      'schema field' => 'primary_contact_id'
    );

    return $info;
  }

}

