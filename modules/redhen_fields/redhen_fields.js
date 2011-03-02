(function ($) {


/**
 * Field instance settings screen: force the 'Display on registration form'
 * checkbox checked whenever 'Required' is checked.
 */
Drupal.behaviors.redhenFieldsDisplayWidget = {
  attach: function (context, settings) {
    $('.redhen-email-widget-item').each(function(index, Element) {
      var text;
      if ($(this).find('.form-type-textfield input').attr('value') === '') {
        $(this).hide();
      }
    });
  }
};



})(jQuery);
