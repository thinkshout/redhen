<?php
/**
 * @file
 * Redhen Contact entity classses
 */

/**
 * The class used for contact entities.
 */
class RedhenContact extends Entity {

  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_contact');
  }

  /**
   * Override buildContent() to add contact properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content['name'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Name'),
      '#items' => array(
        array(
          '#markup' => check_plain($this->first_name) . ' ' . check_plain($this->last_name)
        ),
       ),
      '#classes' => 'field field-label-inline clearfix',
    );
    $content['disposition'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Disposition'),
      '#items' => array(
        array(
          '#markup' => ($this->disposition == REDHEN_DISPOSITION_ACTIVE) ? t('Active') : t('Inactive')
        ),
       ),
      '#classes' => 'field field-label-inline clearfix',
    );

    return entity_get_controller($this->entityType)
			->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $type = redhen_contact_get_types($this->type);
    return $type->label . ': ' . $this->first_name . ' ' . $this->last_name;
  }

}


/**
 * Controls metadata for Redhen contacts
 */
class RedhenContactMetadataController extends EntityDefaultMetadataController {
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['created'] = array(
      'label' => t("Created"),
      'description' => t("The date the contact was created."),
      'type' => 'date',
      'schema field' => 'created',
    );
    $properties['updated'] = array(
      'label' => t("Updated"),
      'description' => t("The date the contact was updated."),
      'type' => 'date',
      'schema field' => 'updated',
    );
    $properties['user'] = array(
      'label' => t("User"),
      'type' => 'user',
      'description' => t("The Drupal user assocaited with the contact."),
      'schema field' => 'user_uid'
    );
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the contact record."),
      'schema field' => 'author_uid'
    );

    return $info;
  }
}

